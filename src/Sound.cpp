#include "static_headers.h"

#include "GameLogic.h"
#include "InterfaceLogic.h"
#include "Sound.h"
#include "Crosy.h"
#include "Time.h"

FMOD::System * Sound::system = NULL;
FMOD::Sound * Sound::samples[SAMPLE_COUNT];
FMOD::Channel * Sound::channel = NULL;
FMOD::Sound * Sound::musicSound = NULL;
FMOD::Channel * Sound::musicChannel = NULL;
unsigned int Sound::version = 0;
void * Sound::extradriverdata = NULL;
DIR * Sound::musicDir = NULL;
bool Sound::initialized = false;
std::string Sound::musicPath = Crosy::getExePath() + "Music/";
std::string Sound::soundPath = Crosy::getExePath() + "Sounds/";

void Sound::init()
{
  assert(!initialized);

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
            result = system->createSound((soundPath + "drop.wav").c_str(), FMOD_DEFAULT, 0, samples + smpDrop);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "left.wav").c_str(), FMOD_DEFAULT, 0, samples + smpLeft);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "right.wav").c_str(), FMOD_DEFAULT, 0, samples + smpRight);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "hold.wav").c_str(), FMOD_DEFAULT, 0, samples + smpHold);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "down.wav").c_str(), FMOD_DEFAULT, 0, samples + smpDown);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "wipe.wav").c_str(), FMOD_DEFAULT, 0, samples + smpWipe);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "levelup.wav").c_str(), FMOD_DEFAULT, 0, samples + smpLevelUp);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "ui_move.wav").c_str(), FMOD_DEFAULT, 0, samples + smpUiClick);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "ui_enter.wav").c_str(), FMOD_DEFAULT, 0, samples + smpUiAnimIn);
            assert(result == FMOD_OK);
            result = system->createSound((soundPath + "ui_move.wav").c_str(), FMOD_DEFAULT, 0, samples + smpUiAnimOut);
            assert(result == FMOD_OK);

            musicDir = opendir(musicPath.c_str());
            assert(musicDir);

            if (musicDir)
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

  updateSfx();
  updateMusic();

  FMOD_RESULT result = system->update();
  assert(result == FMOD_OK);
}

void Sound::quit()
{
  initialized = false;
  FMOD_RESULT result;

  closedir(musicDir);

  for (int i = 0; i < SAMPLE_COUNT; i++)
  {
    result = samples[i]->release();
    assert(result == FMOD_OK);
  }

  if (musicSound)
  {
    result = musicSound->release();
    assert(result == FMOD_OK);
  }

  result = system->release();       
  assert(result == FMOD_OK);
}

void Sound::play(Sample sample)
{
  FMOD_RESULT result = system->playSound(samples[sample], 0, false, &channel);
  assert(result == FMOD_OK);

  if (result == FMOD_OK)
  {
    result = channel->setVolume(InterfaceLogic::settingsLogic.getSoundVolume());
    assert(result == FMOD_OK);
  }
}

void Sound::updateSfx()
{
  static int curFigureId = GameLogic::curFigure.id;
  static int curFigureX = GameLogic::curFigureX;
  static int curFigureAngle = GameLogic::curFigure.angle;

  if (curFigureId != GameLogic::curFigure.id)
  {
    curFigureId = GameLogic::curFigure.id;
    curFigureX = GameLogic::curFigureX;
    curFigureAngle = GameLogic::curFigure.angle;
  }

  if (curFigureX != GameLogic::curFigureX)
  {
    if (GameLogic::curFigureX < curFigureX)
      play(smpLeft);
    else
      play(smpRight);

    curFigureX = GameLogic::curFigureX;
  }

  if (curFigureAngle != GameLogic::curFigure.angle)
  {
    if (GameLogic::curFigure.angle < curFigureAngle)
      play(smpLeft);
    else
      play(smpRight);

    curFigureAngle = GameLogic::curFigure.angle;
  }

  static int holdFigureId = GameLogic::holdFigure.id;

  if (holdFigureId != GameLogic::holdFigure.id)
  {
    play(smpHold);
    holdFigureId = GameLogic::holdFigure.id;
  }

  static unsigned int fastDownCounter = GameLogic::fastDownCounter;

  if (GameLogic::fastDownCounter > fastDownCounter)
  {
    play(smpDown);
    fastDownCounter = GameLogic::fastDownCounter;
  }

  static int dropTrailsCount = GameLogic::getDropTrailsCount();

  if (GameLogic::getDropTrailsCount() > dropTrailsCount)
    play(smpDrop);

  dropTrailsCount = GameLogic::getDropTrailsCount();

  static int deletedRowsCount = GameLogic::getDeletedRowsCount();
  static int curLevel = GameLogic::curLevel;

  if (GameLogic::getDeletedRowsCount() > deletedRowsCount)
  {
    if (GameLogic::curLevel > curLevel)
      play(smpLevelUp);
    else
      play(smpWipe);
  }

  deletedRowsCount = GameLogic::getDeletedRowsCount();
  curLevel = GameLogic::curLevel;

  static MenuLogic::State mainMenuState = InterfaceLogic::mainMenu.state;

  if (mainMenuState != InterfaceLogic::mainMenu.state)
  {
    if (InterfaceLogic::mainMenu.state == MenuLogic::stShowing)
      play(smpUiAnimIn);
    else if (InterfaceLogic::mainMenu.state == MenuLogic::stHiding)
      play(smpUiAnimOut);

    mainMenuState = InterfaceLogic::mainMenu.state;
  }

  static MenuLogic::State inGameMenuState = InterfaceLogic::inGameMenu.state;

  if (inGameMenuState != InterfaceLogic::inGameMenu.state)
  {
    if (InterfaceLogic::inGameMenu.state == MenuLogic::stShowing)
      play(smpUiAnimIn);
    else if (InterfaceLogic::inGameMenu.state == MenuLogic::stHiding)
      play(smpUiAnimOut);

    inGameMenuState = InterfaceLogic::inGameMenu.state;
  }

  static MenuLogic::State quitConfirmationMenuState = InterfaceLogic::quitConfirmationMenu.state;

  if (quitConfirmationMenuState != InterfaceLogic::quitConfirmationMenu.state)
  {
    if (InterfaceLogic::quitConfirmationMenu.state == MenuLogic::stShowing)
      play(smpUiAnimIn);
    else if (InterfaceLogic::quitConfirmationMenu.state == MenuLogic::stHiding)
      play(smpUiAnimOut);

    quitConfirmationMenuState = InterfaceLogic::quitConfirmationMenu.state;
  }

  static MenuLogic::State restartConfirmationMenuState = InterfaceLogic::restartConfirmationMenu.state;

  if (restartConfirmationMenuState != InterfaceLogic::restartConfirmationMenu.state)
  {
    if (InterfaceLogic::restartConfirmationMenu.state == MenuLogic::stShowing)
      play(smpUiAnimIn);
    else if (InterfaceLogic::restartConfirmationMenu.state == MenuLogic::stHiding)
      play(smpUiAnimOut);

    restartConfirmationMenuState = InterfaceLogic::restartConfirmationMenu.state;
  }

  static MenuLogic::State exitToMainConfirmationMenuState = InterfaceLogic::exitToMainConfirmationMenu.state;

  if (exitToMainConfirmationMenuState != InterfaceLogic::exitToMainConfirmationMenu.state)
  {
    if (InterfaceLogic::exitToMainConfirmationMenu.state == MenuLogic::stShowing)
      play(smpUiAnimIn);
    else if (InterfaceLogic::exitToMainConfirmationMenu.state == MenuLogic::stHiding)
      play(smpUiAnimOut);

    exitToMainConfirmationMenuState = InterfaceLogic::exitToMainConfirmationMenu.state;
  }

  static SettingsLogic::State settingsState = InterfaceLogic::settingsLogic.state;

  if (settingsState != InterfaceLogic::settingsLogic.state)
  {
    if (InterfaceLogic::settingsLogic.state == SettingsLogic::stShowing)
      play(smpUiAnimIn);
    else if (InterfaceLogic::settingsLogic.state == SettingsLogic::stHiding)
      play(smpUiAnimOut);
    else if (InterfaceLogic::settingsLogic.state == SettingsLogic::stKeyWaiting)
      play(smpUiClick);

    settingsState = InterfaceLogic::settingsLogic.state;
  }

  static LeaderboardLogic::State leaderboardState = InterfaceLogic::leaderboardLogic.state;

  if (leaderboardState != InterfaceLogic::leaderboardLogic.state)
  {
    if (InterfaceLogic::leaderboardLogic.state == LeaderboardLogic::stShowing)
      play(smpUiAnimIn);
    else if (InterfaceLogic::leaderboardLogic.state == LeaderboardLogic::stHiding)
      play(smpUiAnimOut);

    leaderboardState = InterfaceLogic::leaderboardLogic.state;
  }

  static float soundVolume = InterfaceLogic::settingsLogic.getSoundVolume();

  if (abs(soundVolume - InterfaceLogic::settingsLogic.getSoundVolume()) > 0.001)
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

    soundVolume = InterfaceLogic::settingsLogic.getSoundVolume();
  }

  static float musicVolume = InterfaceLogic::settingsLogic.getMusicVolume();

  if (musicVolume != InterfaceLogic::settingsLogic.getMusicVolume())
  {
    if (musicChannel)
      musicChannel->setVolume(InterfaceLogic::settingsLogic.getMusicVolume());

    musicVolume = InterfaceLogic::settingsLogic.getMusicVolume();
  }
}

void Sound::updateMusic()
{
  FMOD_RESULT result;

  if (musicChannel)
  {
    bool isPlaying = false;
    result = musicChannel->isPlaying(&isPlaying);
    assert(result == FMOD_OK);

    if (!isPlaying)
    {
      result = musicSound->release();
      assert(result == FMOD_OK);
      musicSound = NULL;
      musicChannel = NULL;
    }
  }

  if (!musicSound)
  {
    static double lastTryTimer = 0.0;
    const double retryTimer = 1.0;

    if (Time::timer - lastTryTimer > retryTimer)
    {
      lastTryTimer = Time::timer;
      int dirPassCnt = 2;

      while (dirPassCnt)
      {
        if (dirent * entry = readdir(musicDir))
        {
          const char * fileName = entry->d_name;

          if (!strcmp(fileName, ".") || !strcmp(fileName, ".."))
            continue;

          int nameLen = entry->d_namlen;
          result = system->createSound((musicPath + fileName).c_str(), FMOD_DEFAULT | FMOD_CREATESTREAM, NULL, &musicSound);
          //assert(result == FMOD_OK);

          if (result == FMOD_OK)
          {
            result = system->playSound(musicSound, NULL, false, &musicChannel);
            assert(result == FMOD_OK);
            result = musicChannel->setVolume(InterfaceLogic::settingsLogic.getMusicVolume());
            assert(result == FMOD_OK);

            if (result == FMOD_OK)
              break;
            else
            {
              result = musicSound->release();
              assert(result == FMOD_OK);
            }
          }
        }
        else
        {
          rewinddir(musicDir);
          dirPassCnt--;
        }
      }
    }
  }
}
