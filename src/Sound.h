#pragma once

class Sound
{
public:
  enum Sample 
  { 
    smpDrop, 
    smpLeft, 
    smpRight, 
    smpHold, 
    smpDown, 
    smpWipe, 
    smpCountdown, 
    smpLevelUp, 
    smpUiClick, 
    smpUiAnimIn, 
    smpUiAnimOut, 
    smpMusic, 
    SAMPLE_COUNT 
  };

  static const int musicLoopBeginMs  = 51;
  static const int musicLoopEndMs = 112992;
  static void init();
  static void update();
  static void quit();
  static void play(Sample sample);

private:
  static FMOD::Sound * samples[SAMPLE_COUNT];
  static FMOD::System * system;
  static FMOD::Channel * musicChannel;
  static unsigned int version;
  static void * extradriverdata;
  static bool initialized;
  static std::string soundPath;
  static int lastFigureId;
  static int lastFigureX;
  static int lastFigureAngle;
  static int lastHoldFigureId;
  static unsigned int lastFastDownCounter;
  static unsigned int lastDropTrailCounter;
  static int lastDeletedRowsCount;
  static int lastLevel;
  static int lastCountdownTimeLeft;
  static GameLogic::State lastGameState;
  static MenuLogic::State lastMainMenuState;
  static MenuLogic::State lastInGameMenuState;
  static MenuLogic::State lastQuitConfirmationMenuState;
  static MenuLogic::State lastRestartConfirmationMenuState;
  static MenuLogic::State lastExitToMainConfirmationMenuState;
  static SettingsLogic::State lastSettingsState;
  static LeaderboardLogic::State lastLeaderboardState;
  static float lastSoundVolume;
  static float lastMusicVolume;

  Sound();
  ~Sound();

  static void resetLastState();
};

