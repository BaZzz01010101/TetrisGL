#pragma once

#include "mm_time.h"

#ifndef MMC_REC_DEVICE
#  define MMC_REC_DEVICE ""
#endif

#ifndef MMC_PLAY_DEVICE
#  define MMC_PLAY_DEVICE ""
#endif

#ifndef MMC_MIDI_DEVICE
#  define MMC_MIDI_DEVICE ""
#endif

#ifndef MMC_FREQ
#  define MMC_FREQ 44100
#endif

#ifndef MMC_BUF_SIZE
#  define MMC_BUF_SIZE 1024
#endif

#ifndef MMC_REC_CHANNELS
#  define MMC_REC_CHANNELS 2
#endif

#ifndef MMC_PLAY_CHANNELS
#  define MMC_PLAY_CHANNELS 2
#endif

#ifndef MMC_PLAY_BUFFERS
#  define MMC_PLAY_BUFFERS 2
#endif



struct BufferSettings
{
  int channels;
  int freq;
  double invChannels;
  double invFreq; // == sample time
  double sampleTime;
};

enum MidiMiniType
{
  MMTYPE_UNKNOWN = 0,
  MMTYPE_NOTE = 1,
  MMTYPE_CONTROL = 2,
  MMTYPE_RESET = 3,
  MMTYPE_KEYPRESS = 4,
};

struct MidiMiniMessage
{
  unsigned char type; // MidiMiniType
  unsigned char d0;
  unsigned char d1;
  unsigned char d2;

  MidiMiniMessage()
  {
    type = MMTYPE_UNKNOWN;
    d0 = 0;
    d1 = 0;
    d2 = 0;
  }

  int getChannel() const // 0..15 !
  {
    return int(d0 & 0xf);
  }

  int getEvent() const
  {
    return int(d0 >> 4);
  }

  int getNote() const
  {
    return int(d1);
  }

  int getVelocity() const
  {
    return int(d2);
  }
};

class IPcmRecordCallback
{
public:
  virtual void onAudioRecorded(float * data, int frames_count, const BufferSettings * buffer_settings) = 0;
};

class IPcmPlayCallback
{
public:
  virtual void onAudioPlay(float * data, int frames_count, const BufferSettings * buffer_settings) = 0;
};

class IMidiCallback
{
public:
  virtual void onMidiReceived(const MidiMiniMessage & msg) = 0;
  virtual void onWriteMidi(MidiMiniMessage * msg_buf, int max_count, int & written_count) = 0;
};

class IAppCallback
{
public:
  virtual void onInitialize() = 0;
  virtual void onFinalize() = 0;
  virtual bool onUpdate() = 0;
};

class Synth : public IPcmRecordCallback, public IPcmPlayCallback, public IMidiCallback, public IAppCallback
{
  bool first;
  double time;

public:
  Synth();
  virtual ~Synth();

  virtual void onAudioRecorded(float *data, int frames_count, const BufferSettings * buffer_settings);
  virtual void onAudioPlay(float *data, int frames_count, const BufferSettings * buffer_settings);
  virtual void onMidiReceived(const MidiMiniMessage & msg);
  virtual void onWriteMidi(MidiMiniMessage * msg_buf, int max_count, int & written_count);
  virtual void onInitialize();
  virtual void onFinalize();
  virtual bool onUpdate();
};

void mmc_setup_term_handlers();

void mmc_run_main_loop(
       IPcmRecordCallback * pcm_record_callback,
       IPcmPlayCallback * pcm_play_callback,
       IMidiCallback * midi_callback,
       IAppCallback * app_callback);

bool mmc_ex_init(
       IPcmRecordCallback * pcm_record_callback,
       IPcmPlayCallback * pcm_play_callback,
       IMidiCallback * midi_callback);

void mmc_ex_finlaize();

bool mmc_ex_need_restart();

