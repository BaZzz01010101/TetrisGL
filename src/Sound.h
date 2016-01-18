#pragma once
class Sound
{
public:
  static const int musicLoopBeginMs  = 51;
  static const int musicLoopEndMs = 112992;
  enum Sample { smpDrop, smpLeft, smpRight, smpHold, smpDown, smpWipe, smpCountdown, smpLevelUp, smpUiClick, smpUiAnimIn, smpUiAnimOut, smpMusic, SAMPLE_COUNT };
  static void init();
  static void update();
  static void quit();
  static void play(Sample sample);

private:
  Sound();
  ~Sound();

  static FMOD::Sound * samples[SAMPLE_COUNT];
  static FMOD::System * system;
  static FMOD::Channel * musicChannel;
  static unsigned int version;
  static void * extradriverdata;
  static bool initialized;
  static std::string soundPath;
};

