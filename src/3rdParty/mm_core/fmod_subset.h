#pragma once

#include <string.h>
#include <memory.h>
#include <vector>

typedef enum
{
    FMOD_OK,
    FMOD_ERR,
    FMOD_RESULT_FORCEINT = 65536
} FMOD_RESULT;

#define FMOD_VERSION 1
#define FMOD_INIT_NORMAL 1
#define FMOD_DEFAULT 0
#define FMOD_CREATESTREAM 0
#define FMOD_LOOP_NORMAL 1
#define FMOD_TIMEUNIT_MS 2

namespace FMOD
{
  FMOD_RESULT System_Create(void *);

  class Sound
  {
  public:
    float * data;
    int length;
    int loopBeginSample;
    int loopEndSample;
    int flags;
    float advance;
    Sound() { memset(this, 0, sizeof(*this)); }
    ~Sound() { release(); }
    FMOD_RESULT release();
    FMOD_RESULT setLoopPoints(int loopBegin, int loopBeginUnits, int loopEnd, int loopEndUnits);
  };

  class Channel
  {
  public:
    float volume;
    bool volumeChanged;

    Channel()
    {
      volume = 1.0f;
      volumeChanged = true;
    }

    FMOD_RESULT setVolume(float volume_)
    {
      volume = volume_;
      volumeChanged = true;
      return FMOD_OK;
    }

    FMOD_RESULT release() { return FMOD_OK; }
  };


  class System
  {
  public:
    FMOD_RESULT getVersion(unsigned int * version) { *version = FMOD_VERSION; return FMOD_OK; }
    FMOD_RESULT init(int max_play_sounds, int, void *);
    FMOD_RESULT setDSPBufferSize(int, int) { return FMOD_OK; };
    FMOD_RESULT createSound( const char * file_name, int flags, void *, Sound ** sound);
    FMOD_RESULT playSound(Sound *& sound, void *, bool, Channel ** channel);
    FMOD_RESULT update();
    FMOD_RESULT release();
  };

};
