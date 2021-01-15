#include "mm_core.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <alsa/asoundlib.h>
#include <signal.h>
#include <sys/poll.h>
#include <sys/time.h>
#include "pthread_sys_lock.h"



#define ONE_DIV_32768 0.00003051757812f
#define ONE_DIV_31BIT 4.656612873077392e-10f
#define MIDI_CLIENT_NAME "mm_core_alsa_midi"
#define MAX_MIDI_POLL_DESC 16
#define MAX_MIDI_MESSAGES 1000

#define MIDI_FATAL(...) \
  do \
  { \
    printf("ERROR: "); \
    printf(__VA_ARGS__); \
    printf("\n"); \
    return false; \
  } while (0)

#define MIDI_FATAL_NULL(...) \
  do \
  { \
    printf("ERROR: "); \
    printf(__VA_ARGS__); \
    printf("\n"); \
    return NULL; \
  } while (0)

#define CHECK_SND(operation, error) \
  do \
  { \
    if (error < 0) \
      MIDI_FATAL("cannot %s (%s)", operation, snd_strerror(err)); \
  } while (0)

#define CHECK_SND_NULL(operation, error) \
  do \
  { \
    if (error < 0) \
      MIDI_FATAL_NULL("cannot %s (%s)", operation, snd_strerror(err)); \
  } while (0)



enum PCMFormat
{
  PCMF_UNKNOWN = 0,
  PCMF_S16,
  PCMF_S24,
  PCMF_FLOAT,
  PCMF_S32,

  PCMF__COUNT
};


class AlsaWrapper
{
  MutexLock recordLock;
  MutexLock playLock;
  MutexLock midiLock;

  bool recordThreadCreated;
  int sysRecordFormat;
  pthread_t recordThread;
  snd_pcm_t * capture_handle;
  char * recordBufferSys;
  float * recordBuffer;
  int recordFrames;
  BufferSettings recordBufferSettings;

  bool playThreadCreated;
  int sysPlayFormat;
  pthread_t playThread;
  snd_pcm_t * playback_handle;
  char * playBufferSys;
  float * playBuffer;
  int playFrames;
  int maxPlayFrames;
  BufferSettings playBufferSettings;

  bool midiThreadCreated;
  pthread_t midiThread;
  snd_seq_t *seq;
  snd_seq_addr_t port;
  int queue;
  int descriptorsCount;
  pollfd pollDescriptors[MAX_MIDI_POLL_DESC];
  MidiMiniMessage midiMessages[MAX_MIDI_MESSAGES];

  IPcmRecordCallback *pcmRecordCallback;
  IPcmPlayCallback *pcmPlayCallback;
  IMidiCallback *midiCallback;
  IAppCallback *appCallback;

  volatile bool exiting;

  struct ThreadExitGuard
  {
    AlsaWrapper * wrap;
    ThreadExitGuard(AlsaWrapper * wrap_) : wrap(wrap_) {}
    ~ThreadExitGuard()
    {
      if (!wrap->exiting)
      {
        wrap->unplannedThreadExit = true;
        printf("\n*** Unplanned exit from thread ***\n");
      }
    }
  };

  static snd_pcm_format_t format_to_alsa_format(int pcm_fromat)
  {
    switch (pcm_fromat)
    {
      case PCMF_S16: return SND_PCM_FORMAT_S16_LE;
      case PCMF_S24: return SND_PCM_FORMAT_S24_3LE;
      case PCMF_FLOAT: return SND_PCM_FORMAT_FLOAT_LE;
      case PCMF_S32: return SND_PCM_FORMAT_S32_LE;
      default: return SND_PCM_FORMAT_S16_LE;
    };
  }

  static const char * format_to_alsa_name(int pcm_fromat)
  {
    switch (pcm_fromat)
    {
      case PCMF_S16: return "SND_PCM_FORMAT_S16_LE";
      case PCMF_S24: return "SND_PCM_FORMAT_S24_3LE";
      case PCMF_FLOAT: return "SND_PCM_FORMAT_FLOAT_LE";
      case PCMF_S32: return "SND_PCM_FORMAT_S32_LE";
      default: return "unknown";
    };
  }

  static int format_stride(int pcm_fromat)
  {
    switch (pcm_fromat)
    {
      case PCMF_S16: return 2;
      case PCMF_S24: return 3;
      case PCMF_FLOAT: return 4;
      case PCMF_S32: return 4;
      default: return 4;
    };
  }

  static void convert_fmt_buf_to_float(int pcm_fromat, void * in_, float * out_, unsigned int samples)
  {
    switch (pcm_fromat)
    {
      case PCMF_S16:
      {
        short * __restrict in = (short *)in_;
        float * __restrict out = out_;

        for (unsigned int i = 0; i < samples; i++)
        {
          *out = (*in) * ONE_DIV_32768;
          out++;
          in++;
        }
      }
      return;

      case PCMF_S24:
      {
        uint8_t * __restrict in = (uint8_t *)in_;
        float * __restrict out = out_;

        for (unsigned int i = 0; i < samples; i++)
        {
          *out = float(int32_t((in[0] << 8) | (in[1] << 16) | (in[2] << 24))) * ONE_DIV_31BIT;
          out++;
          in += 3;
        }
      }
      return;

      case PCMF_FLOAT:
      {
        float * __restrict in = (float *)in_;
        float * __restrict out = out_;

        for (unsigned int i = 0; i < samples; i++)
        {
          *out = *in;
          out++;
          in++;
        }
      }
      return;

      case PCMF_S32:
      {
        int32_t * __restrict in = (int32_t *)in_;
        float * __restrict out = out_;

        for (unsigned int i = 0; i < samples; i++)
        {
          *out = float(*in) * ONE_DIV_31BIT;
          out++;
          in++;
        }
      }
      return;

      default:
        printf("convert_buf_to_float: invlid format\n");
        return;
    };
  }

  static void convert_float_to_fmt_buf(int pcm_fromat, float * in_, void * out_, unsigned int samples)
  {
    switch (pcm_fromat)
    {
      case PCMF_S16:
      {
        float * __restrict in = in_;
        short * __restrict out = (short *)out_;

        for (unsigned int i = 0; i < samples; i++)
        {
          float v = *in;
          *out = short((v < 1.0f ? (v > -1.0f ? v : -1.0f) : 1.0f) * 32766.f);
          //*out = ((iv & 0xff) << 8) | ((iv & 0xff00) >> 8);
          out++;
          in++;
        }
      }
      return;

      case PCMF_S24:
      {
        float * __restrict in = in_;
        uint8_t * __restrict out = (uint8_t *)out_;

        for (unsigned int i = 0; i < samples; i++)
        {
          float v = *in;
          int iv = int((v < 1.0f ? (v > -1.0f ? v : -1.0f) : 1.0f) * 8388600.f);
          out[0] = uint8_t(iv & 0xFF);
          out[1] = uint8_t((iv >> 8) & 0xFF);
          out[2] = uint8_t((iv >> 16) & 0xFF);
          out += 3;
          in++;
        }
      }
      return;

      case PCMF_FLOAT:
      {
        float * __restrict in = in_;
        float * __restrict out = (float *)out_;

        for (unsigned int i = 0; i < samples; i++)
        {
          *out = *in;
          out++;
          in++;
        }
      }
      return;

      case PCMF_S32:
      {
        int32_t * __restrict out = (int32_t *)out_;
        float * __restrict in = in_;

        for (unsigned int i = 0; i < samples; i++)
        {
          *out = int32_t((*in) * 2147479000.0f);
          out++;
          in++;
        }
      }
      return;

      default:
        printf("convert_buf_to_float: invlid format\n");
        return;
    };
  }

public:
  volatile bool unplannedThreadExit;
  bool inited;

  AlsaWrapper()
  {
    inited = false;

    recordThreadCreated = false;
    playThreadCreated = false;
    midiThreadCreated = false;
    exiting = false;

    pcmRecordCallback = NULL;
    pcmPlayCallback = NULL;
    midiCallback = NULL;
    appCallback = NULL;

    capture_handle = NULL;
    recordBuffer = NULL;
    recordBufferSys = NULL;
    recordFrames = 2;

    maxPlayFrames = 4096;
    playFrames = maxPlayFrames;
    playback_handle = NULL;
    playBufferSys = NULL;
    playBuffer = NULL;

    sysRecordFormat = 0;
    sysPlayFormat = 0;

    seq = NULL;
    queue = 0;
    descriptorsCount = 0;
    memset(pollDescriptors, 0, sizeof(pollDescriptors));
    memset(midiMessages, 0, sizeof(midiMessages));

    unplannedThreadExit = false;
  }

  bool reinitMidi(
    const char * midiDeviceName,
    IMidiCallback *midiCallback_
    )
  {
    if (seq)
      snd_seq_close(seq);

    seq = 0;

    // midi
    if (midiDeviceName && *midiDeviceName && midiCallback_)
    {
      int err = 0;

      printf("MIDI: substing = '%s'\n", midiDeviceName);
      if (!initMidiSeq())
        return false;

      if (!findMidiPort(midiDeviceName, port))
        return false;

      err = snd_seq_create_simple_port(seq, MIDI_CLIENT_NAME,
              SND_SEQ_PORT_CAP_WRITE |
              SND_SEQ_PORT_CAP_SUBS_WRITE |
              SND_SEQ_PORT_CAP_READ |
              SND_SEQ_PORT_CAP_DUPLEX ,
              SND_SEQ_PORT_TYPE_MIDI_GENERIC |
              SND_SEQ_PORT_TYPE_APPLICATION);
      CHECK_SND("create port", err);

      err = snd_seq_connect_from(seq, 0, port.client, port.port);
      if (err < 0)
        MIDI_FATAL("Cannot connect from port %d:%d - %s", port.client, port.port, snd_strerror(err));

      err = snd_seq_nonblock(seq, 1);
      CHECK_SND("set nonblock mode", err);

      descriptorsCount = snd_seq_poll_descriptors_count(seq, POLLIN);
      if (descriptorsCount >= MAX_MIDI_POLL_DESC)
        MIDI_FATAL("too many poll descriptors = %d", descriptorsCount);

      err = snd_seq_connect_to(seq, 0, port.client, port.port);
      if (err < 0)
        MIDI_FATAL("Cannot connect to port %d:%d - %s", port.client, port.port, snd_strerror(err));

      queue = snd_seq_alloc_named_queue(seq, MIDI_CLIENT_NAME);
      CHECK_SND("create queue", queue);

      err = snd_seq_start_queue(seq, queue, NULL);
      CHECK_SND("start queue", err);

      midiCallback = midiCallback_;

      if (pthread_create(&midiThread, NULL, midiThreadFunc, this) != 0)
      {
        printf("ERROR: pthread_create failed\n");
        return false;
      }

      midiThreadCreated = true;
    } // midi

    return true;
  }

  bool init(
    const char * recordDeviceName,
    const char * playDeviceName,
    const char * midiDeviceName,
    int desiredFrequency,
    int desiredRecordChannels,
    int desiredPlayChannels,
    int desiredRecordBufferFrames,
    int desiredPlayBufferFrames,
    IPcmRecordCallback *pcmRecordCallback_,
    IPcmPlayCallback *pcmPlayCallback_,
    IMidiCallback *midiCallback_,
    IAppCallback *appCallback_
    )
  {
    unplannedThreadExit = false;

    if (inited)
    {
      printf("ERROR: AlsaWrapper already inited\n");
      return false;
    }

    printf("initializing AlsaWrapper...\n");

    // record
    if (recordDeviceName && *recordDeviceName && pcmRecordCallback_)
    {
      printf("record: '%s'\n", recordDeviceName);

      int err = 0;
      recordFrames = desiredRecordBufferFrames;
      unsigned int rate = desiredFrequency;
      //snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
      snd_pcm_hw_params_t *hw_params = NULL;

      if ((err = snd_pcm_open (&capture_handle, recordDeviceName, SND_PCM_STREAM_CAPTURE, 0)) < 0)
      {
        printf("ERROR: cannot open audio device %s (%s)\n", recordDeviceName, snd_strerror (err));
        return false;
      }

      if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
      {
        printf("ERROR: cannot allocate hardware parameter structure (%s)\n", snd_strerror (err));
        return false;
      }

      if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0)
      {
        printf("ERROR: cannot initialize hardware parameter structure (%s)\n",
          snd_strerror (err));
        return false;
      }

      if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
      {
        printf("ERROR: cannot set access type (%s)\n", snd_strerror (err));
        return false;
      }

      for (int f = PCMF_UNKNOWN + 1; f < PCMF__COUNT; f++)
      {
        snd_pcm_format_t format = format_to_alsa_format(f);
        err = snd_pcm_hw_params_set_format(capture_handle, hw_params, format);
        if (err >= 0)
        {
          printf("  accepted format: (%d) %s\n", f, format_to_alsa_name(f));
          sysRecordFormat = f;
          break;
        }
      }

      if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params,
        format_to_alsa_format(sysRecordFormat))) < 0)
      {
        printf("ERROR: cannot set sample format (%s)\n", snd_strerror (err));
        return false;
      }

      if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0)
      {
        printf("ERROR: cannot set sample rate (%s)\n", snd_strerror (err));
        return false;
      }

      if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, desiredRecordChannels)) < 0)
      {
        printf("ERROR: cannot set record channel count (%s)\n", snd_strerror (err));
        return false;
      }


      if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0)
      {
        printf("ERROR: cannot set parameters (%s)\n", snd_strerror (err));
        return false;
      }

      snd_pcm_hw_params_free (hw_params);

      if ((err = snd_pcm_prepare (capture_handle)) < 0)
      {
        printf("ERROR: cannot prepare audio interface for use (%s)\n", snd_strerror (err));
        return false;
      }

      recordBufferSettings.freq = rate;
      recordBufferSettings.invFreq = 1.0 / rate;
      recordBufferSettings.sampleTime = 1.0 / rate;
      recordBufferSettings.channels = desiredRecordChannels;
      recordBufferSettings.invChannels = 1.0 / desiredRecordChannels;

      recordBufferSys = new char[recordFrames * desiredRecordChannels * format_stride(sysRecordFormat)];
      recordBuffer = new float[recordFrames * desiredRecordChannels];
      pcmRecordCallback = pcmRecordCallback_;

      if (pthread_create(&recordThread, NULL, recordThreadFunc, this) != 0)
      {
        printf("ERROR: pthread_create failed\n");
        return false;
      }

      recordThreadCreated = true;
    } // record


    // play
    if (playDeviceName && *playDeviceName && pcmPlayCallback_)
    {
      printf("playback: '%s'\n", playDeviceName);

      int err = 0;
      playFrames = desiredPlayBufferFrames;
      unsigned int rate = desiredFrequency;
      int dir = 0;
      //snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
      snd_pcm_hw_params_t *hw_params = NULL;
      snd_pcm_sw_params_t *sw_params = NULL;

      if ((err = snd_pcm_open (&playback_handle, playDeviceName, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
      {
        printf("ERROR: cannot open audio device %s (%s)\n", playDeviceName, snd_strerror(err));
        return false;
      }

      if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
      {
        printf("ERROR: cannot allocate hardware parameter structure (%s)\n", snd_strerror(err));
        return false;
      }

      if ((err = snd_pcm_hw_params_any(playback_handle, hw_params)) < 0)
      {
        printf("ERROR: cannot initialize hardware parameter structure (%s)\n", snd_strerror(err));
        return false;
      }

      if ((err = snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
      {
        printf("ERROR: cannot set access type (%s)\n", snd_strerror(err));
        return false;
      }

      for (int f = PCMF_UNKNOWN + 1; f < PCMF__COUNT; f++)
      {
        snd_pcm_format_t format = format_to_alsa_format(f); 
        err = snd_pcm_hw_params_set_format(playback_handle, hw_params, format);
        if (err >= 0)
        {
          printf("  accepted format: (%d) %s\n", f, format_to_alsa_name(f));
          sysPlayFormat = f;
          break; 
        }
      }

      if ((err = snd_pcm_hw_params_set_format(playback_handle, hw_params,
        format_to_alsa_format(sysPlayFormat))) < 0)
      {
        printf("ERROR: cannot set sample format (%s)\n", snd_strerror (err));
        return false;
      }


      if ((err = snd_pcm_hw_params_set_rate_near(playback_handle, hw_params, &rate, 0)) < 0)
      {
        printf("ERROR: cannot set sample rate (%s)\n", snd_strerror(err));
        return false;
      }

      if ((err = snd_pcm_hw_params_set_channels(playback_handle, hw_params, desiredPlayChannels)) < 0)
      {
        printf("ERROR: cannot set play channel count = %d (%s)\n", desiredPlayChannels, snd_strerror(err));
        return false;
      }



      int nperiods = MMC_PLAY_BUFFERS;
      int period = desiredPlayBufferFrames;
      snd_pcm_uframes_t real_buffer_size = 0;
      snd_pcm_uframes_t real_period_size = 0;

      unsigned buffer_time = 1000000 * (uint64_t)period * nperiods / rate;
      err = snd_pcm_hw_params_set_buffer_time_near(playback_handle, hw_params, &buffer_time, &dir);
      if (err < 0)
      {
        printf("Unable to set buffer time %i for playback: %s\n",  1000000 * period * nperiods / rate, snd_strerror(err));
        return false;
      }

      err = snd_pcm_hw_params_get_buffer_size( hw_params, &real_buffer_size );
      if (err < 0)
      {
        printf("Unable to get buffer size back: %s\n", snd_strerror(err));
        return false;
      }

      if (int(real_buffer_size) != nperiods * period)
      {
        printf( "WARNING: buffer size does not match: (requested %d, got %d)\n", nperiods * period, (int) real_buffer_size );
      }

      /* set the period time */
      unsigned period_time = 1000000 * (uint64_t)period / rate;
      err = snd_pcm_hw_params_set_period_time_near(playback_handle, hw_params, &period_time, &dir);
      if (err < 0)
      {
        printf("Unable to set period time %i for playback: %s\n", 1000000 * period / rate, snd_strerror(err));
        return false;
      }

      err = snd_pcm_hw_params_get_period_size(hw_params, &real_period_size, NULL );
      if (err < 0)
      {
        printf("Unable to get period size back: %s\n", snd_strerror(err));
        return false;
      }

      if (int(real_period_size) != period )
      {
        printf( "WARNING: period size does not match: (requested %i, got %i)\n", period, (int)real_period_size );
      }



      if ((err = snd_pcm_hw_params(playback_handle, hw_params)) < 0)
      {
        printf("ERROR: cannot set parameters (%s)\n", snd_strerror(err));
        return false;
      }

      snd_pcm_hw_params_free(hw_params);

      if ((err = snd_pcm_sw_params_malloc(&sw_params)) < 0)
      {
        printf("ERROR: cannot allocate software parameters structure (%s)\n", snd_strerror(err));
        return false;
      }

      if ((err = snd_pcm_sw_params_current(playback_handle, sw_params)) < 0)
      {
        printf("ERROR: cannot initialize software parameters structure (%s)\n", snd_strerror(err));
        return false;
      }

      if ((err = snd_pcm_sw_params_set_avail_min(playback_handle, sw_params, playFrames)) < 0)
      {
        printf("ERROR: cannot set minimum available count (%s)\n", snd_strerror(err));
        return false;
      }

      if ((err = snd_pcm_sw_params_set_start_threshold(playback_handle, sw_params, 0U)) < 0)
      {
        printf("ERROR: cannot set start mode (%s)\n", snd_strerror(err));
        return false;
      }

      if ((err = snd_pcm_sw_params(playback_handle, sw_params)) < 0)
      {
        printf("ERROR: cannot set software parameters (%s)\n", snd_strerror(err));
        return false;
      }

      if ((err = snd_pcm_prepare(playback_handle)) < 0)
      {
        printf("ERROR: cannot prepare audio interface for use (%s)\n", snd_strerror(err));
        return false;
      }

      playBufferSettings.freq = rate;
      playBufferSettings.invFreq = 1.0 / rate;
      playBufferSettings.sampleTime = 1.0 / rate;
      playBufferSettings.channels = desiredPlayChannels;
      playBufferSettings.invChannels = 1.0 / desiredPlayChannels;

      playBufferSys = new char[playFrames * desiredPlayChannels * format_stride(sysPlayFormat)];
      //printf("format_stride = %d\n", int(format_stride(sysPlayFormat)) );
      playBuffer = new float[playFrames * desiredPlayChannels];
      pcmPlayCallback = pcmPlayCallback_;

      if (pthread_create(&playThread, NULL, playThreadFunc, this) != 0)
      {
        printf("ERROR: pthread_create failed\n");
        return false;
      }

      playThreadCreated = true;
    } // play


    // midi
    if (midiDeviceName && *midiDeviceName && midiCallback_)
    {
      int err = 0;

      printf("MIDI: substing = '%s'\n", midiDeviceName);
      if (!initMidiSeq())
        return false;

      if (!findMidiPort(midiDeviceName, port))
        return false;

      err = snd_seq_create_simple_port(seq, MIDI_CLIENT_NAME,
              SND_SEQ_PORT_CAP_WRITE |
              SND_SEQ_PORT_CAP_SUBS_WRITE |
              SND_SEQ_PORT_CAP_READ |
              SND_SEQ_PORT_CAP_DUPLEX ,
              SND_SEQ_PORT_TYPE_MIDI_GENERIC |
              SND_SEQ_PORT_TYPE_APPLICATION);
      CHECK_SND("create port", err);

      err = snd_seq_connect_from(seq, 0, port.client, port.port);
      if (err < 0)
        MIDI_FATAL("Cannot connect from port %d:%d - %s", port.client, port.port, snd_strerror(err));

      err = snd_seq_nonblock(seq, 1);
      CHECK_SND("set nonblock mode", err);

      descriptorsCount = snd_seq_poll_descriptors_count(seq, POLLIN);
      if (descriptorsCount >= MAX_MIDI_POLL_DESC)
        MIDI_FATAL("too many poll descriptors = %d", descriptorsCount);

      err = snd_seq_connect_to(seq, 0, port.client, port.port);
      if (err < 0)
        MIDI_FATAL("Cannot connect to port %d:%d - %s", port.client, port.port, snd_strerror(err));

      queue = snd_seq_alloc_named_queue(seq, MIDI_CLIENT_NAME);
      CHECK_SND("create queue", queue);

      err = snd_seq_start_queue(seq, queue, NULL);
      CHECK_SND("start queue", err);

      midiCallback = midiCallback_;

      if (pthread_create(&midiThread, NULL, midiThreadFunc, this) != 0)
      {
        printf("ERROR: pthread_create failed\n");
        return false;
      }

      midiThreadCreated = true;
    } // midi

    appCallback = appCallback_;

    printf("AlsaWrapper inited\n");
    inited = true;

    if (appCallback)
      appCallback->onInitialize();

    return true;
  }

  ~AlsaWrapper()
  {
    exiting = true;
    printf("\nclosing AlsaWrapper...\n");

    if (appCallback)
      appCallback->onFinalize();

    if (recordThreadCreated)
    {
      printf("closing record...\n");
      pthread_join(recordThread, NULL);
      snd_pcm_close(capture_handle);
    }

    if (playThreadCreated)
    {
      printf("closing playback...\n");
      pthread_join(playThread, NULL);
      snd_pcm_close(playback_handle);
    }

    if (midiThreadCreated)
    {
      printf("closing MIDI...\n");
      pthread_join(midiThread, NULL);
    }

    if (seq)
      snd_seq_close(seq);

    delete[] recordBufferSys;
    delete[] recordBuffer;
    delete[] playBufferSys;
    delete[] playBuffer;
  }

  static void * midiThreadFunc(void * ptr)
  {
    AlsaWrapper * wrap = (AlsaWrapper *)ptr;
    ThreadExitGuard guard(wrap);
    while (!wrap->exiting)
    {
      snd_seq_poll_descriptors(wrap->seq, wrap->pollDescriptors, wrap->descriptorsCount, POLLIN);
      if (poll(wrap->pollDescriptors, wrap->descriptorsCount, 10) < 0)  // 10 msec
      {
        printf("ERROR:[M] poll error\n");
        break;
      }

      int err = 0;

      do
      {
        snd_seq_event_t *event = NULL;
        err = snd_seq_event_input(wrap->seq, &event);
        if (err < 0)
          break;

        if (event)
        {
          MidiMiniMessage msg;
          if (event->type == SND_SEQ_EVENT_NOTEON || event->type == SND_SEQ_EVENT_NOTEOFF)
          {
            msg.type = MMTYPE_NOTE;
            msg.d0 = event->data.note.channel;
            msg.d1 = event->data.note.note;
            msg.d2 = event->data.note.velocity;
          }
          else if (event->type == SND_SEQ_EVENT_CONTROLLER)
          {
            msg.type = MMTYPE_CONTROL;
            msg.d0 = event->data.control.channel;
            msg.d1 = event->data.control.param;
            msg.d2 = event->data.control.value;
          }
          else if (event->type == SND_SEQ_EVENT_KEYPRESS)
          {
            msg.type = MMTYPE_KEYPRESS;
            msg.d0 = event->data.note.channel;
            msg.d1 = event->data.note.note;
            msg.d2 = event->data.note.velocity;
          }
          else
          {
            msg.type = MMTYPE_NOTE;
            msg.d0 = 0x7f;
            msg.d1 = 0x7f;
            msg.d2 = 0x7f;
          }

          if (wrap->midiCallback)
            wrap->midiCallback->onMidiReceived(msg);
        }
      }
      while (err > 0);


      int count = 0;
      if (wrap->midiCallback)
        wrap->midiCallback->onWriteMidi(wrap->midiMessages, MAX_MIDI_MESSAGES, count);

      if (count > 0)
      {
        snd_seq_event_t ev;
        snd_seq_ev_clear(&ev);
        ev.queue = wrap->queue;
        ev.source.port = 0;
        ev.flags = SND_SEQ_TIME_STAMP_TICK;

        for (int i = 0; i < count; i++)
        {
          const MidiMiniMessage & msg = wrap->midiMessages[i];
          if (msg.type == MMTYPE_NOTE)
          {
            ev.type = SND_SEQ_EVENT_NOTEON;
            ev.time.tick = 0;
            ev.dest = wrap->port;
            snd_seq_ev_set_fixed(&ev);
            ev.data.note.channel = msg.d0;
            ev.data.note.note = msg.d1;
            ev.data.note.velocity = msg.d2;
          }
          else if (msg.type == MMTYPE_CONTROL)
          {
            ev.type = SND_SEQ_EVENT_CONTROLLER;
            ev.time.tick = 0;
            ev.dest = wrap->port;
            snd_seq_ev_set_fixed(&ev);
            ev.data.control.channel = msg.d0;
            ev.data.control.param = msg.d1;
            ev.data.control.value = msg.d2;
          }
          else if (msg.type == MMTYPE_RESET)
          {
            ev.type = SND_SEQ_EVENT_RESET;
            ev.time.tick = 0;
            ev.dest = wrap->port;
            snd_seq_ev_set_fixed(&ev);
          }
          else
          {
            printf("ERROR:[M] unknown msg type\n");
          }

          err = snd_seq_event_output(wrap->seq, &ev);
          CHECK_SND_NULL("output event", err);
        }

        err = snd_seq_drain_output(wrap->seq);
        CHECK_SND_NULL("drain output", err);
      }
    }

    return NULL;
  }

  static int outstream_xrun_recovery(snd_pcm_t * handle, int err)
  {
    if (err == -EPIPE)
    {
      err = snd_pcm_prepare(handle);
      //if (err >= 0)
      //  outstream->underflow_callback(outstream);
    }
    else if (err == -ESTRPIPE)
    {
      while ((err = snd_pcm_resume(handle)) == -EAGAIN) {
        // wait until suspend flag is released
        poll(NULL, 0, 1);
      }
      if (err < 0)
        err = snd_pcm_prepare(handle);
      //if (err >= 0)
      //  outstream->underflow_callback(outstream);
    }
    return err;
  }

  static void * playThreadFunc(void * ptr)
  {
    int fails = 0;
    unsigned cnt = 0;
    bool written = false;
    AlsaWrapper * wrap = (AlsaWrapper *)ptr;
    ThreadExitGuard guard(wrap);
    while (!wrap->exiting)
    {
      cnt++;
      if ((cnt & 0xff) == 0 && fails > 0)
        fails--;

      int err = 0;
      int frames_to_deliver = 0;

      snd_pcm_state_t state = snd_pcm_state(wrap->playback_handle);
      if (state == SND_PCM_STATE_SETUP)
      {
        if ((err = snd_pcm_prepare(wrap->playback_handle)) < 0)
          printf("ERROR:[p] SND_PCM_STATE_SETUP: snd_pcm_prepare failed\n");
        continue;
      }

      if (state == SND_PCM_STATE_XRUN)
      {
        if ((err = outstream_xrun_recovery(wrap->playback_handle, -EPIPE)) < 0)
          printf("ERROR:[p] SND_PCM_STATE_SETUP: outstream_xrun_recovery failed (1)\n");
        continue;
      }

      if (state == SND_PCM_STATE_SUSPENDED)
      {
        if ((err = outstream_xrun_recovery(wrap->playback_handle, -ESTRPIPE)) < 0)
          printf("ERROR:[p] SND_PCM_STATE_SETUP: outstream_xrun_recovery failed (2)\n");
        continue;
      }

      if (state == SND_PCM_STATE_PREPARED)
      {
        if ((err = snd_pcm_start(wrap->playback_handle)) < 0)
        {
          printf("ERROR:[p] snd_pcm_start error (%s)\n", snd_strerror(err));
          break;
        }
        continue;
      }


      if (written && (err = snd_pcm_wait(wrap->playback_handle, 6000)) < 0) // wait for 6 sec
      {
        if (err == -EPIPE)
        {
	        err = snd_pcm_prepare(wrap->playback_handle);
	        if (err < 0)
          {
            printf("ERROR:[p] an xrun occured 2\n");
	        }          
        }
        else
          printf("ERROR:[p] poll failed (%s)\n", strerror (errno));
        fails++;
        if (fails > 10)
          break;
      }

      if ((frames_to_deliver = snd_pcm_avail_update(wrap->playback_handle)) < 0)
      {
        if (frames_to_deliver == -EPIPE)
          printf("ERROR:[p] an xrun occured\n");
        else
          printf("ERROR:[p] unknown ALSA avail update return value (%d)\n", frames_to_deliver);

        if ((err = snd_pcm_recover(wrap->playback_handle, frames_to_deliver, 0)) < 0)
        {
          printf("ERROR:[p] playback recover failed (%s)\n", snd_strerror(err));
          fails++;
          if (fails > 10)
            break;
        }

        frames_to_deliver = wrap->playFrames / 2;
      }

      frames_to_deliver = frames_to_deliver > wrap->playFrames ? wrap->playFrames : frames_to_deliver;

      if (wrap->pcmPlayCallback && frames_to_deliver > 0)
      {
        //ScopedLocker lock(wrap->playLock);
        wrap->pcmPlayCallback->onAudioPlay(wrap->playBuffer, frames_to_deliver, &wrap->playBufferSettings);

        unsigned int cnt = frames_to_deliver * wrap->playBufferSettings.channels;
        convert_float_to_fmt_buf(wrap->sysPlayFormat, wrap->playBuffer, wrap->playBufferSys, cnt);

        if ((err = snd_pcm_writei(wrap->playback_handle, wrap->playBufferSys, frames_to_deliver)) < 0)
        {
          sleep_msec(100);
          fails++;
          if (fails > 10000)
          {
            printf("ERROR:[p] write failed (%s)\n", snd_strerror(err));
            break;
          }
        }
        else
        {
          written = true;
        }

      }
    }

    return NULL;
  }

  static void * recordThreadFunc(void * ptr)
  {
    AlsaWrapper * wrap = (AlsaWrapper *)ptr;
    ThreadExitGuard guard(wrap);
    while (!wrap->exiting)
    {
      int res = 0;

    /*  int err = 0;
      if ((err = snd_pcm_wait(wrap->capture_handle, 2000)) < 0) // wait for 2 sec
      {
        printf("ERROR:[r] poll failed (%s)\n", strerror (errno));
        break;
      }*/

      if ((res = snd_pcm_readi(wrap->capture_handle, wrap->recordBufferSys, wrap->recordFrames)) < 0)
      {
        printf("ERROR:[r] snd_pcm_readi returned: %s\n", snd_strerror(res));
        return NULL;
      }

      {
        unsigned int cnt = res * wrap->recordBufferSettings.channels;
        convert_fmt_buf_to_float(wrap->sysRecordFormat, wrap->recordBufferSys, wrap->recordBuffer, cnt);

        //ScopedLocker lock(wrap->recordLock);
        if (wrap->pcmRecordCallback)
          wrap->pcmRecordCallback->onAudioRecorded(wrap->recordBuffer, res, &wrap->recordBufferSettings);
      }
    }
    return NULL;
  }

  bool initMidiSeq()
  {
    int err = 0;

    err = snd_seq_open(&seq, "default", SND_SEQ_OPEN_DUPLEX, 0);
    CHECK_SND("open sequencer", err);

    err = snd_seq_set_client_name(seq, MIDI_CLIENT_NAME);
    CHECK_SND("set client name", err);

    return true;
  }

  bool findMidiPort(const char * port_name, snd_seq_addr_t & out_port)
  {
    out_port.client = 0;
    out_port.port = 0;

    if (!port_name || !*port_name)
      return false;

    snd_seq_client_info_t *cinfo;
    snd_seq_port_info_t *pinfo;

    snd_seq_client_info_alloca(&cinfo);
    snd_seq_port_info_alloca(&pinfo);

    snd_seq_client_info_set_client(cinfo, -1);
    while (snd_seq_query_next_client(seq, cinfo) >= 0)
    {
      int client = snd_seq_client_info_get_client(cinfo);

      snd_seq_port_info_set_client(pinfo, client);
      snd_seq_port_info_set_port(pinfo, -1);
      while (snd_seq_query_next_port(seq, pinfo) >= 0)
      {
        unsigned int caps = SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ;
        if ((snd_seq_port_info_get_capability(pinfo) & caps) != caps)
          continue;

        const char * name = snd_seq_port_info_get_name(pinfo);
        if (name && strstr(name, port_name))
        {
          out_port.client = snd_seq_port_info_get_client(pinfo);
          out_port.port = snd_seq_port_info_get_port(pinfo);

          printf("MIDI: found port name = %s\n", snd_seq_port_info_get_name(pinfo));
          printf("MIDI: found port = %u:%u\n", unsigned(out_port.client), unsigned(out_port.port));
          return true;
        }
      }
    }

    printf("MIDI: port with substing '%s' not found\n", port_name);
    return false;
  }

  bool update()
  {
    if (appCallback)
      return appCallback->onUpdate();

    return true;
  }

};



class AlsaTest : public IPcmRecordCallback, public IPcmPlayCallback, public IMidiCallback
{
  bool first;
  double time;
public:

  AlsaTest()
  {
    first = true;
    time = 0.0;
  }

  virtual void onAudioRecorded(float *data, int frames_count, const BufferSettings * buffer_settings)
  {
    (void)data;
    (void)frames_count;
    (void)buffer_settings;
    printf("R");
  }

  virtual void onAudioPlay(float *data, int frames_count, const BufferSettings * buffer_settings)
  {
    //  printf("p");
    for (int i = 0; i < frames_count; i++, data += buffer_settings->channels)
    {
      time += 0.001;
      if (time > 0.1)
        time = 0.0;

      for (int c = 0; c < buffer_settings->channels; c++)
        data[c] = time; 
    }
  }

  virtual void onMidiReceived(const MidiMiniMessage & msg)
  {
    printf("MIDI: %d %d %d %d\n", msg.type, msg.d0, msg.d1, msg.d2);
  }

  virtual void onWriteMidi(MidiMiniMessage * msg_buf, int max_count, int & written_count)
  {
    if (first)
    {
      printf("#");
      first = false;

      if (written_count < max_count)
      {
        MidiMiniMessage & r = msg_buf[written_count++];
        r.type = MMTYPE_RESET;
        r.d0 = 0;
        r.d1 = 0;
        r.d2 = 0;
      }

      if (written_count < max_count)
      {
        MidiMiniMessage & msg = msg_buf[written_count++];
        msg.type = MMTYPE_NOTE;
        msg.d0 = 12;
        msg.d1 = 9;
        msg.d2 = 0x00;
      }

      if (written_count < max_count)
      {
        MidiMiniMessage & msg = msg_buf[written_count++];
        msg.type = MMTYPE_NOTE;
        msg.d0 = 5;
        msg.d1 = 71;
        msg.d2 = 127;
      }
    }
  }
};



static AlsaWrapper * wrap = NULL;

static void term_sig_handler(int)
{
  delete wrap;
  wrap = NULL;
  exit(1);
}

void mmc_setup_term_handlers()
{
  signal(SIGINT, term_sig_handler);
  signal(SIGTERM, term_sig_handler);
}


bool reinit_midi = false;

void mmc_run_main_loop(
       IPcmRecordCallback * pcm_record_callback,
       IPcmPlayCallback * pcm_play_callback,
       IMidiCallback * midi_callback,
       IAppCallback * app_callback)
{
  int failCount = 0;

  for (;;)
  {
    if (!wrap)
      wrap = new AlsaWrapper;

    if (wrap && !wrap->inited)
    {
      bool res = wrap->init(
        MMC_REC_DEVICE, // rec
        MMC_PLAY_DEVICE, // play
        MMC_MIDI_DEVICE, // midi (substring)
        MMC_FREQ,
        MMC_REC_CHANNELS, // rec
        MMC_PLAY_CHANNELS, // play
        MMC_BUF_SIZE, // rec
        MMC_BUF_SIZE, // play
        pcm_record_callback,
        pcm_play_callback,
        midi_callback,
        app_callback
      );

      if (!res)
      {
        printf("init failed\n");
        delete wrap;
        wrap = NULL;
        failCount++;
      }
    }

    if (wrap && wrap->unplannedThreadExit)
    {
      printf("Unplanned Thread Exit\n");
      delete wrap;
      wrap = NULL;
    }

    if (wrap)
    {
      if (!wrap->update())
      {
        printf("Exit from update\n");
        break;
      }

      if (reinit_midi)
      {
        reinit_midi = !wrap->reinitMidi(MMC_MIDI_DEVICE, midi_callback);
      }

      if (failCount > 0)
        failCount--;
    }

    int sleepTime = 2;
    if (failCount > 8)
      sleepTime = 20;
    if (failCount > 16)
      sleepTime = 50;
    if (failCount > 32)
      sleepTime = 200;
    if (failCount > 64)
      sleepTime = 2000;
    if (failCount > 100)
    {
      printf("Too many fails\n");
      break;
    }

    if (failCount > 32)
      printf("Fail count = %d\n", failCount);

    sleep_msec(sleepTime);
  }

  delete wrap;
  wrap = NULL;
}




bool mmc_ex_init(
       IPcmRecordCallback * pcm_record_callback,
       IPcmPlayCallback * pcm_play_callback,
       IMidiCallback * midi_callback)
{
  if (!wrap)
    wrap = new AlsaWrapper;

  const char * playDevice = getenv("MMC_PLAY_DEVICE");
  if (!playDevice)
    playDevice = MMC_PLAY_DEVICE;

  const char * recordDevice = getenv("MMC_REC_DEVICE");
  if (!recordDevice)
    recordDevice = MMC_REC_DEVICE;

  const char * midiDevice = getenv("MMC_MIDI_DEVICE");
  if (!midiDevice)
    midiDevice = MMC_MIDI_DEVICE;

  if (wrap && !wrap->inited)
  {
    if (!*playDevice)
      playDevice = "default";

    bool res = wrap->init(
      recordDevice, // rec
      playDevice, // play
      midiDevice, // midi (substring)
      MMC_FREQ,
      MMC_REC_CHANNELS, // rec
      MMC_PLAY_CHANNELS, // play
      MMC_BUF_SIZE, // rec
      MMC_BUF_SIZE, // play
      pcm_record_callback,
      pcm_play_callback,
      midi_callback,
      NULL
    );


    if (!res)
    {
      printf("ERROR: Sound initialization failed\n");
      delete wrap;
      wrap = NULL;
      return false;
    }
  }

  return true;
}

bool mmc_ex_need_restart()
{
  return wrap && wrap->unplannedThreadExit;
}

void mmc_ex_finlaize()
{
  delete wrap;
  wrap = NULL;
}

