#include "static_headers.h"

#include "SettingsLogic.h"
#include "Globals.h"
#include "Time.h"


SettingsLogic::SettingsLogic() :
  saveConfirmationMenu(MenuLogic::resBack),
  changed(false),
  state(stHidden),
  transitionProgress(0.0f),
  soundVolume(0.66f),
  musicVolume(0.75f),
  selectedControl(ctrlSoundVolume),
  highlightedControl(ctrlNone),
  selectedAction(Binding::doNothing),
  highlightedAction(Binding::doNothing),
  backButtonHighlighted(false)
{
  saveConfirmationMenu.add("SAVE", MenuLogic::resSave, true);
  saveConfirmationMenu.add("DON'T SAVE", MenuLogic::resDontSave);
  saveConfirmationMenu.add("BACK", MenuLogic::resBack);
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
  changed = false;
}

void SettingsLogic::save()
{
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
