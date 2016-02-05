#include "static_headers.h"

#include "GameLogic.h"
#include "InterfaceLogic.h"
#include "Sound.h"
#include "Crosy.h"
#include "Globals.h"
#include "Time.h"

FMOD::System * Sound::system = NULL;
FMOD::Sound * Sound::samples[SAMPLE_COUNT];
FMOD::Channel * Sound::musicChannel = NULL;
unsigned int Sound::version = 0;
void * Sound::extradriverdata = NULL;
bool Sound::initialized = false;
std::string Sound::soundPath = Crosy::getExePath() + "Sounds/";
int Sound::lastFigureId = 0;
int Sound::lastFigureX = 0;
int Sound::lastFigureAngle = 0;
int Sound::lastHoldFigureId = 0;
unsigned int Sound::lastFastDownCounter = 0;
unsigned int Sound::lastDropTrailCounter = 0;
int Sound::lastDeletedRowsCount = 0;
int Sound::lastLevel = 0;
int Sound::lastCountdownTimeLeft = 0;
GameLogic::State Sound::lastGameState = GameLogic::stStopped;
MenuLogic::State Sound::lastMainMenuState = MenuLogic::stHidden;
MenuLogic::State Sound::lastInGameMenuState = MenuLogic::stHidden;
MenuLogic::State Sound::lastQuitConfirmationMenuState = MenuLogic::stHidden;
MenuLogic::State Sound::lastRestartConfirmationMenuState = MenuLogic::stHidden;
MenuLogic::State Sound::lastExitToMainConfirmationMenuState = MenuLogic::stHidden;
SettingsLogic::State Sound::lastSettingsState = SettingsLogic::stHidden;
LeaderboardLogic::State Sound::lastLeaderboardState = LeaderboardLogic::stHidden;
float Sound::lastSoundVolume = 0.0f;
float Sound::lastMusicVolume = 0.0f;

void Sound::init()
{
  assert(!initialized);
  resetLastState();

  if (!initialized)
  {
    memset(samples, 0, sizeof(samples));
    FMOD_RESULT result = FMOD_OK;
    result = FMOD::System_Create(&system);
    assert(result == FMOD_OK);

    if (result == FMOD_OK)
    {
      result = system->setDSPBufferSize(1024, 2);
      assert(result == FMOD_OK);

      if (result == FMOD_OK)
      {
        result = system->getVersion(&version);
        assert(result == FMOD_OK);
        assert(version >= FMOD_VERSION);

        if (result == FMOD_OK && version >= FMOD_VERSION)
        {
          result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
          assert(result == FMOD_OK);

          if (result == FMOD_OK)
          {
            result = system->createSound((soundPath + "drop.wav").c_str(), FMOD_DEFAULT, 0, 
                                         samples + smpDrop);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "left.wav").c_str(), FMOD_DEFAULT, 0, 
                                         samples + smpLeft);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "right.wav").c_str(), FMOD_DEFAULT, 0, 
                                         samples + smpRight);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "hold.wav").c_str(), FMOD_DEFAULT, 0, 
                                         samples + smpHold);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "down.wav").c_str(), FMOD_DEFAULT, 0, 
                                         samples + smpDown);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "wipe.wav").c_str(), FMOD_DEFAULT, 0, 
                                         samples + smpWipe);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "levelup.wav").c_str(), FMOD_DEFAULT, 0, 
                                         samples + smpLevelUp);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "countdown.wav").c_str(), FMOD_DEFAULT, 0, 
                                         samples + smpCountdown);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "ui_move.wav").c_str(), FMOD_DEFAULT, 0, 
                                         samples + smpUiClick);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "ui_enter.wav").c_str(), FMOD_DEFAULT, 0, 
                                         samples + smpUiAnimIn);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "ui_move.wav").c_str(), FMOD_DEFAULT, 0, 
                                         samples + smpUiAnimOut);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "music.mp3").c_str(), 
                                         FMOD_DEFAULT | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, 
                                         NULL, samples + smpMusic);
            assert(result == FMOD_OK);

            if (result == FMOD_OK)
            {
              result = samples[smpMusic]->setLoopPoints(musicLoopBeginMs , FMOD_TIMEUNIT_MS, 
                                                        musicLoopEndMs, FMOD_TIMEUNIT_MS);
              assert(result == FMOD_OK);
              result = system->playSound(samples[smpMusic], NULL, false, &musicChannel);
              assert(result == FMOD_OK);
              result = musicChannel->setVolume(InterfaceLogic::settingsLogic.getMusicVolume());
              assert(result == FMOD_OK);
            }

            initialized = true;
          }
        }
      }
    }
  }
}


void Sound::update()
{
  if (!initialized)
    return;

  if (GameLogic::state != lastGameState)
  {
    if (GameLogic::state == GameLogic::stStopped)
    {
      lastFigureId = GameLogic::curFigure.id;
      lastFigureX = GameLogic::curFigureX;
      lastFigureAngle = GameLogic::curFigure.angle;
      lastHoldFigureId = GameLogic::holdFigure.id;
      lastFastDownCounter = GameLogic::fastDownCounter;
      lastDropTrailCounter = GameLogic::dropTrailCounter;
      lastDeletedRowsCount = GameLogic::getDeletedRowsCount();
      lastLevel = GameLogic::curLevel;
    }

    lastGameState = GameLogic::state;
  }

  if (lastFigureId != GameLogic::curFigure.id)
  {
    lastFigureId = GameLogic::curFigure.id;
    lastFigureX = GameLogic::curFigureX;
    lastFigureAngle = GameLogic::curFigure.angle;
  }

  if (GameLogic::curFigureX != lastFigureX)
  {
    if (GameLogic::curFigureX < lastFigureX)
      play(smpLeft);
    else if (GameLogic::curFigureX > lastFigureX)
      play(smpRight);

    lastFigureX = GameLogic::curFigureX;
  }

  if (GameLogic::curFigure.angle != lastFigureAngle)
  {
    if (GameLogic::curFigure.angle < lastFigureAngle)
      play(smpLeft);
    else if (GameLogic::curFigure.angle > lastFigureAngle)
      play(smpRight);

    lastFigureAngle = GameLogic::curFigure.angle;
  }

  if (GameLogic::holdFigure.id != lastHoldFigureId)
  {
    play(smpHold);
    lastHoldFigureId = GameLogic::holdFigure.id;
  }

  if (GameLogic::fastDownCounter != lastFastDownCounter)
  {
    play(smpDown);
    lastFastDownCounter = GameLogic::fastDownCounter;
  }

  if (GameLogic::dropTrailCounter > lastDropTrailCounter)
    play(smpDrop);

  lastDropTrailCounter = GameLogic::dropTrailCounter;

  if (GameLogic::getDeletedRowsCount() > lastDeletedRowsCount)
  {
    if (GameLogic::curLevel > lastLevel)
      play(smpLevelUp);
    else
      play(smpWipe);
  }

  lastDeletedRowsCount = GameLogic::getDeletedRowsCount();
  lastLevel = GameLogic::curLevel;

  if (lastCountdownTimeLeft != (int)GameLogic::countdownTimeLeft)
  {
    play(smpCountdown);
    lastCountdownTimeLeft = (int)GameLogic::countdownTimeLeft;
  }

  if (lastMainMenuState != InterfaceLogic::mainMenu.state)
  {
    if (InterfaceLogic::mainMenu.state == MenuLogic::stShowing)
      play(smpUiAnimIn);
    else if (InterfaceLogic::mainMenu.state == MenuLogic::stHiding)
      play(smpUiAnimOut);

    lastMainMenuState = InterfaceLogic::mainMenu.state;
  }

  if (lastInGameMenuState != InterfaceLogic::inGameMenu.state)
  {
    if (InterfaceLogic::inGameMenu.state == MenuLogic::stShowing)
      play(smpUiAnimIn);
    else if (InterfaceLogic::inGameMenu.state == MenuLogic::stHiding)
      play(smpUiAnimOut);

    lastInGameMenuState = InterfaceLogic::inGameMenu.state;
  }

  if (lastQuitConfirmationMenuState != InterfaceLogic::quitConfirmationMenu.state)
  {
    if (InterfaceLogic::quitConfirmationMenu.state == MenuLogic::stShowing)
      play(smpUiAnimIn);
    else if (InterfaceLogic::quitConfirmationMenu.state == MenuLogic::stHiding)
      play(smpUiAnimOut);

    lastQuitConfirmationMenuState = InterfaceLogic::quitConfirmationMenu.state;
  }

  if (lastRestartConfirmationMenuState != InterfaceLogic::restartConfirmationMenu.state)
  {
    if (InterfaceLogic::restartConfirmationMenu.state == MenuLogic::stShowing)
      play(smpUiAnimIn);
    else if (InterfaceLogic::restartConfirmationMenu.state == MenuLogic::stHiding)
      play(smpUiAnimOut);

    lastRestartConfirmationMenuState = InterfaceLogic::restartConfirmationMenu.state;
  }

  if (lastExitToMainConfirmationMenuState != InterfaceLogic::exitToMainConfirmationMenu.state)
  {
    if (InterfaceLogic::exitToMainConfirmationMenu.state == MenuLogic::stShowing)
      play(smpUiAnimIn);
    else if (InterfaceLogic::exitToMainConfirmationMenu.state == MenuLogic::stHiding)
      play(smpUiAnimOut);

    lastExitToMainConfirmationMenuState = InterfaceLogic::exitToMainConfirmationMenu.state;
  }

  if (lastSettingsState != InterfaceLogic::settingsLogic.state)
  {
    if (InterfaceLogic::settingsLogic.state == SettingsLogic::stShowing)
      play(smpUiAnimIn);
    else if (InterfaceLogic::settingsLogic.state == SettingsLogic::stHiding)
      play(smpUiAnimOut);
    else if (InterfaceLogic::settingsLogic.state == SettingsLogic::stKeyWaiting)
      play(smpUiClick);

    lastSettingsState = InterfaceLogic::settingsLogic.state;
  }

  if (lastLeaderboardState != InterfaceLogic::leaderboardLogic.state)
  {
    if (InterfaceLogic::leaderboardLogic.state == LeaderboardLogic::stShowing)
      play(smpUiAnimIn);
    else if (InterfaceLogic::leaderboardLogic.state == LeaderboardLogic::stHiding)
      play(smpUiAnimOut);

    lastLeaderboardState = InterfaceLogic::leaderboardLogic.state;
  }

  if (abs(lastSoundVolume - InterfaceLogic::settingsLogic.getSoundVolume()) > 0.001)
  {
    if (InterfaceLogic::settingsLogic.state == SettingsLogic::stVisible)
    {
      static double lastPlayedTimer = 0.0;
      const double replayTime = 0.125;

      if (Time::timer - lastPlayedTimer > replayTime)
      {
        play(smpDrop);
        lastPlayedTimer = Time::timer;
      }
    }

    lastSoundVolume = InterfaceLogic::settingsLogic.getSoundVolume();
  }

  if (lastMusicVolume != InterfaceLogic::settingsLogic.getMusicVolume())
  {
    if (musicChannel)
      musicChannel->setVolume(InterfaceLogic::settingsLogic.getMusicVolume());

    lastMusicVolume = InterfaceLogic::settingsLogic.getMusicVolume();
  }

  FMOD_RESULT result = system->update();
  assert(result == FMOD_OK);
}


void Sound::quit()
{
  initialized = false;
  FMOD_RESULT result;

  for (int i = 0; i < SAMPLE_COUNT; i++)
  {
    result = samples[i]->release();
    assert(result == FMOD_OK);
  }

  result = system->release();       
  assert(result == FMOD_OK);
}


void Sound::play(Sample sample)
{
  FMOD::Channel * channel = NULL;
  FMOD_RESULT result = system->playSound(samples[sample], 0, false, &channel);
  assert(result == FMOD_OK);

  if (result == FMOD_OK)
  {
    result = channel->setVolume(InterfaceLogic::settingsLogic.getSoundVolume());
    assert(result == FMOD_OK);
  }
}

void Sound::resetLastState()
{
  lastFigureId = GameLogic::curFigure.id;
  lastFigureX = GameLogic::curFigureX;
  lastFigureAngle = GameLogic::curFigure.angle;
  lastHoldFigureId = GameLogic::holdFigure.id;
  lastFastDownCounter = GameLogic::fastDownCounter;
  lastDropTrailCounter = GameLogic::dropTrailCounter;
  lastDeletedRowsCount = GameLogic::getDeletedRowsCount();
  lastLevel = GameLogic::curLevel;
  lastCountdownTimeLeft = (int)GameLogic::countdownTimeLeft;
  lastGameState = GameLogic::state;
  lastMainMenuState = InterfaceLogic::mainMenu.state;
  lastInGameMenuState = InterfaceLogic::inGameMenu.state;
  lastQuitConfirmationMenuState = InterfaceLogic::quitConfirmationMenu.state;
  lastRestartConfirmationMenuState = InterfaceLogic::restartConfirmationMenu.state;
  lastExitToMainConfirmationMenuState = InterfaceLogic::exitToMainConfirmationMenu.state;
  lastSettingsState = InterfaceLogic::settingsLogic.state;
  lastLeaderboardState = InterfaceLogic::leaderboardLogic.state;
  lastSoundVolume = InterfaceLogic::settingsLogic.getSoundVolume();
  lastMusicVolume = InterfaceLogic::settingsLogic.getMusicVolume();
}
