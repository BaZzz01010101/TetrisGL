#include "static_headers.h"

#include "SettingsLogic.h"
#include "Globals.h"
#include "Time.h"
#include "Crosy.h"

const char * SettingsLogic::fileName = "settings.dat";

SettingsLogic::SettingsLogic() :
  saveConfirmationMenu(MenuLogic::resBack),
  changed(false),
  state(stHidden),
  transitionProgress(0.0f),
  soundVolume(0.5f),
  musicVolume(0.5f),
  selectedControl(ctrlSoundVolume),
  highlightedControl(ctrlNone),
  selectedAction(Binding::doNothing),
  highlightedAction(Binding::doNothing),
  backButtonHighlighted(false)
{
  saveConfirmationMenu.add("SAVE", MenuLogic::resSave);
  saveConfirmationMenu.add("DON'T SAVE", MenuLogic::resDontSave);
  saveConfirmationMenu.add("BACK", MenuLogic::resBack, true);
  load();
}


SettingsLogic::~SettingsLogic()
{
}

SettingsLogic::Result SettingsLogic::update()
{
  Result retResult = resNone;

  switch (state)
  {
  case stHidden:
    state = stShowing;
    break;

  case stShowing:
    if ((transitionProgress += Time::timerDelta / Globals::settingsShowingTime) >= 1.0f)
    {
      transitionProgress = 1.0f;
      state = stVisible;
    }

    break;

  case stVisible:
    break;

  case stHiding:
    if ((transitionProgress -= Time::timerDelta / Globals::settingsHidingTime) <= 0.0f)
    {
      transitionProgress = 0.0f;
      state = stHidden;
      retResult = resClose;
    }

    break;

  case stSaveConfirmation:
    saveConfirmationUpdate();
    break;

  case stKeyWaiting:
    break;

  default:
    assert(0);
    break;
  }

  return retResult;
}

void SettingsLogic::saveConfirmationUpdate()
{
  switch (saveConfirmationMenu.update())
  {
  case MenuLogic::resSave:        saveAndExit();     break;
  case MenuLogic::resDontSave:    cancelAndExit();   break;
  case MenuLogic::resBack:        state = stVisible; break;
  default: break;
  }
}

void SettingsLogic::escape()
{
  assert(state == stVisible);

  if (changed)
    state = stSaveConfirmation;
  else
    state = stHiding;
}

void SettingsLogic::load()
{
  std::string fileName = Crosy::getExePath() + SettingsLogic::fileName;
  FILE * file = fopen(fileName.c_str(), "rb+");

  if (file)
  {
    int success = fread(&saveData, sizeof(saveData), 1, file);
    fclose(file);

    if (success)
    {
      uint32_t * plainData = (uint32_t *)&saveData;
      int chunkCount = (sizeof(saveData)-sizeof(saveData.checksum)) / sizeof(uint32_t);
      uint32_t checksum = 0;

      for (int i = 0; i < chunkCount; i++)
        checksum ^= plainData[i];

      if (checksum == saveData.checksum)
      {
        soundVolume = saveData.soundVolume;
        musicVolume = saveData.musicVolume;

        for (Binding::Action action = Binding::FIRST_ACTION; action < Binding::ACTION_COUNT; action++)
          Binding::setKeyBinding(saveData.actionKeys[action], action);
      }
      else save();
    }
    else save();
  }
  else save();

  changed = false;
}

void SettingsLogic::save()
{
  std::string fileName = Crosy::getExePath() + SettingsLogic::fileName;
  FILE * file = fopen(fileName.c_str(), "wb+");
  assert(file);

  if (file)
  {
    saveData.soundVolume = soundVolume;
    saveData.musicVolume = musicVolume;
    saveData.dummy = 0;
    saveData.checksum = 0;

    for (Binding::Action action = Binding::FIRST_ACTION; action < Binding::ACTION_COUNT; action++)
      saveData.actionKeys[action] = Binding::getActionKey(action);

    uint32_t * plainData = (uint32_t *)&saveData;
    int chunkCount = (sizeof(saveData)-sizeof(saveData.checksum)) / sizeof(uint32_t);

    for (int i = 0; i < chunkCount; i++)
      saveData.checksum ^= plainData[i];

    int success = fwrite(&saveData, sizeof(saveData), 1, file);
    assert(success);
    fclose(file);
  }

  changed = false;
}

void SettingsLogic::setSoundVolume(float volume)
{
  soundVolume = volume;
  changed = true;
}

void SettingsLogic::setMusicVolume(float volume)
{
  musicVolume = volume;
  changed = true;
}

void SettingsLogic::setCurrentActionKey(Key key)
{
  assert(selectedControl == ctrlKeyBindTable);
  assert(state == stKeyWaiting);

  Binding::setKeyBinding(key, selectedAction);
  changed = true;
}

float SettingsLogic::getSoundVolume()
{
  return soundVolume;
}

float SettingsLogic::getMusicVolume()
{
  return musicVolume;
}

Key getKeyBind(Binding::Action action)
{

  return Binding::getActionKey(action);
}

void SettingsLogic::saveAndExit()
{
  save();
  state = stHiding;
}

void SettingsLogic::cancelAndExit()
{
  load();
  state = stHiding;
}

void SettingsLogic::selectNext()
{
  switch (selectedControl)
  {
  case ctrlSoundVolume:
    selectedControl = ctrlMusicVolume;
    break;
  case ctrlMusicVolume:
    selectedControl = ctrlKeyBindTable;
    selectedAction = Binding::FIRST_ACTION;
    break;
  case ctrlKeyBindTable:
    if (++selectedAction == Binding::ACTION_COUNT)
      selectedControl = ctrlSoundVolume;
    break;
  default: assert(0);
  }
}

void SettingsLogic::selectPrevious()
{
  switch (selectedControl)
  {
  case ctrlSoundVolume:
    selectedControl = ctrlKeyBindTable;
    selectedAction = Binding::ACTION_COUNT - 1;
    break;
  case ctrlMusicVolume:
    selectedControl = ctrlSoundVolume;
    break;
  case ctrlKeyBindTable:
    if (--selectedAction == Binding::doNothing)
      selectedControl = ctrlMusicVolume;
    break;
  default: assert(0);
  }
}

