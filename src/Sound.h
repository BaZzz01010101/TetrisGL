#pragma once
class Sound
{
public:
  enum Sample { smpDrop, smpLeft, smpRight, smpHold, smpDown, smpWipe, smpLevelUp, smpUiClick, smpUiAnimIn, smpUiAnimOut, SAMPLE_COUNT };
  static void init();
  static void update();
  static void quit();
  static void play(Sample sample);

private:
  Sound();
  ~Sound();

  static FMOD::Sound * samples[SAMPLE_COUNT];
  static FMOD::System * system;
  static FMOD::Sound * musicSound;
  static FMOD::Channel * channel;
  static FMOD::Channel * musicChannel;
  static unsigned int version;
  static void * extradriverdata;
  static DIR * musicDir;
  static bool initialized;
  static std::string musicPath;
  static std::string soundPath;

  static void loadSound(const char * fileName);
  static void updateSfx();
  static void updateMusic();
};

