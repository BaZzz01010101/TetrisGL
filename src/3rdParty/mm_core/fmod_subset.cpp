#include "fmod_subset.h"

#include "mm_core.h"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#define MINIMP3_IMPLEMENTATION
#define MINIMP3_FLOAT_OUTPUT
#include "minimp3.h"
#include "minimp3_ex.h"
#include <signal.h>
#include "pthread_sys_lock.h"

MutexLock cs;

static bool quited = false;

#define MAX_FMOD_CHANNELS 1024
static FMOD::Channel fmod_channels[MAX_FMOD_CHANNELS];
static int used_channels = 0;

#define MAX_FMOD_SOUNDS 1024
static FMOD::Sound fmod_sounds[MAX_FMOD_SOUNDS];
static int used_sounds = 0;

struct PlaySnd
{
  FMOD::Channel * originChannel;
  float * data;
  double playPos;
  float advance;
  float volume;
  int beginSample;
  int endSample;
  bool loop;
  bool playing;

  void reset()
  {
    memset(this, 0, sizeof(*this));
  }
};


#define MAX_FMOD_PLAYING_SOUNDS 32
static PlaySnd playSnd[MAX_FMOD_PLAYING_SOUNDS];


namespace FMOD
{

  FMOD_RESULT Sound::setLoopPoints(int loopBegin, int loopBeginUnits, int loopEnd, int loopEndUnits)
  {
    if (loopBeginUnits == FMOD_TIMEUNIT_MS)
      loopBegin = int(loopBegin * 0.001 * MMC_FREQ / advance);

    if (loopEndUnits == FMOD_TIMEUNIT_MS)
      loopEnd = int(loopEnd * 0.001 * MMC_FREQ / advance);

    if (loopBegin > length)
      loopBegin = length;

    if (loopEnd > length)
      loopEnd = length;

    loopBeginSample = loopBegin;
    loopEndSample = loopEnd;

    return FMOD_OK;
  }


  FMOD_RESULT Sound::release()
  {
    ScopedLocker lock(cs);
    // TODO: stop at playSnd
    delete[] data;
    data = nullptr;
    return FMOD_OK;
  }


  class PlayCallback: public IPcmPlayCallback
  {
  public:
    virtual void onAudioPlay(float * data, int frames_count, const BufferSettings * buffer_settings)
    {
      memset(data, 0, frames_count * buffer_settings->channels * sizeof(float));
      
      ScopedLocker lock(cs);

      for (PlaySnd & s : playSnd)
        if (s.playing)
        {
          float volume = s.volume;
          if (s.originChannel)
            volume *= s.originChannel->volume;
          double pos = s.playPos;
          const float * buf = s.data;

          for (int d = 0; d < frames_count; d++)
          {
            int ipos = int(pos);
            float t = pos - ipos;
            float value0 = buf[ipos];
            float value1 = buf[ipos + 1];
            float value = value0 + (value1 - value0) * t;
            data[d] += value * volume;
            pos += s.advance;
            if (pos >= s.endSample)
            {
              if (s.loop)
                pos = s.beginSample;
              else
              {
                s.playing = false;
                break;
              }
            }
          }

          s.playPos = pos;
        }

      for (int i = 0; i < frames_count; i++)
      {
        if (data[i] < -1.0f)
          data[i] = -1.0f;
        else if (data[i] > 1.0f)
          data[i] = 1.0f;
      }

      if (buffer_settings->channels == 2)
        for (int i = frames_count - 1; i >= 0; i--)
        {
          data[i * 2] = data[i];
          data[i * 2 + 1] = data[i];
        }

      if (buffer_settings->channels == 4)
        for (int i = frames_count - 1; i >= 0; i--)
        {
          data[i * 2] = data[i];
          data[i * 2 + 1] = data[i];
          data[i * 2 + 2] = data[i];
          data[i * 2 + 3] = data[i];
        }
    }
  } pcm_play_callback;


  FMOD_RESULT System::init(int max_play_sounds, int, void *)
  {
    return FMOD_OK;
  }


  FMOD_RESULT System::createSound( const char * file_name, int flags, void *, Sound ** sound)
  {
    if (used_sounds >= MAX_FMOD_SOUNDS || !file_name)
      return FMOD_ERR;

    if (!(*sound))
    {
      *sound = &(fmod_sounds[used_sounds]);
      used_sounds++;

      Sound & snd = **sound;
      snd.flags = flags;

      if (strstr(file_name, ".mp3"))
      {
        mp3dec_t mp3d;
        mp3dec_file_info_t info;
        memset(&mp3d, 0, sizeof(mp3d));
        memset(&info, 0, sizeof(info));
        int res = mp3dec_load(&mp3d, file_name, &info, NULL, NULL);
        if (res)
        {
          printf("ERROR: cannot load .mp3-file '%s'\n", file_name);
          free(info.buffer);
          return FMOD_ERR;
        }

        snd.advance = float(info.hz ? info.hz : 44100) / MMC_FREQ;

        if (info.channels == 1)
        {
          snd.length = info.samples;
          snd.data = new float[snd.length + 1];
          memcpy(snd.data, info.buffer, sizeof(float) * snd.length);
          snd.data[snd.length] = snd.data[snd.length - 1];
        }
        else if (info.channels == 2)
        {
          snd.length = info.samples / 2;
          snd.data = new float[snd.length + 1];
          for (size_t i = 0; i < snd.length; i++)
            snd.data[i] = (info.buffer[i * 2] + info.buffer[i * 2 + 1]) * 0.5f;
          snd.data[snd.length] = snd.data[snd.length - 1];
        }
        else
        {
          printf("ERROR: invalid number of channels in '%s'\n", file_name);
          free(info.buffer);
          return FMOD_ERR;
        }

        free(info.buffer);
      }
      else if (strstr(file_name, ".wav"))
      {
        unsigned int channels = 1;
        unsigned int sampleRate = 44100;
        drwav_uint64 totalSampleCount = 0;
        float * buf = drwav_open_and_read_file_f32(file_name, &channels, &sampleRate, &totalSampleCount);

        snd.advance = float(sampleRate) / MMC_FREQ;

        if (channels == 1)
        {
          snd.length = totalSampleCount;
          snd.data = new float[snd.length + 1];
          memcpy(snd.data, buf, sizeof(float) * snd.length);
          snd.data[snd.length] = snd.data[snd.length - 1];
        }
        else if (channels == 2)
        {
          snd.length = totalSampleCount / 2;
          snd.data = new float[snd.length + 1];
          for (size_t i = 0; i < snd.length; i++)
            snd.data[i] = (buf[i * 2] + buf[i * 2 + 1]) * 0.5f;
          snd.data[snd.length] = snd.data[snd.length - 1];            
        }
        else
        {
          free(buf);
          return FMOD_ERR;
        }
      }
      else
      {
        printf("Invalid file type '%s', expected .wav, .mp3\n", file_name);
        return FMOD_ERR;
      }

      return FMOD_OK;
    }

    return FMOD_OK;
  }


  FMOD_RESULT System::playSound(Sound *& sound, void *, bool, Channel ** channel)
  {
    if (used_channels >= MAX_FMOD_CHANNELS)
      return FMOD_ERR;

    if (!(*channel))
    {
      *channel = &(fmod_channels[used_channels]);
      used_channels++;
    }

    if (sound->length <= 2)
      return FMOD_OK;

    ScopedLocker lock(cs);

    for (PlaySnd & s : playSnd)
      if (!s.playing)
      {
        s.originChannel = *channel;
        s.data = sound->data;
        s.advance = sound->advance;
        s.playPos = sound->loopBeginSample;
        s.volume = 1.0f;
        s.playing = true;
        s.loop = !!(sound->flags & FMOD_LOOP_NORMAL);
        s.beginSample = s.loop ? sound->loopBeginSample : 0;
        s.endSample = s.loop ? sound->loopEndSample : sound->length;

        break;
      }

    return FMOD_OK;
  }


  FMOD_RESULT System::update()
  {
    return FMOD_OK;
  }


  FMOD_RESULT System::release()
  {
    ScopedLocker lock(cs);
    quited = true;
    return FMOD_OK;
  }
  

  FMOD_RESULT System_Create(void *)
  {
    mmc_setup_term_handlers();
    mmc_ex_init(NULL, &pcm_play_callback, NULL);
    return FMOD_OK;
  }
};

