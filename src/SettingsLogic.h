#pragma once

#include "MenuLogic.h"
#include "Binding.h"

class SettingsLogic
{
public:
  enum State { stHidden, stShowing, stVisible, stSaveConfirmation, stHiding };
  enum Result { resNone, resClose };
  enum Control { ctrlNone, ctrlSoundVolume, ctrlMusicVolume, ctrlKeyBindTable, ctrlBackButton };

  MenuLogic saveConfirmationMenu;
  State state;
  float transitionProgress;
  Control selectedControl;
  Control highlightedControl;
  Binding::Action selectedAction;
  Binding::Action highlightedAction;
  bool backButtonHighlighted;

  SettingsLogic();
  ~SettingsLogic();

  Result update();
  void escape();
  void load();
  void save();
  void setSoundVolume(float volume);
  void setMusicVolume(float volume);
  void setKeyBind(Binding::Action action, Key key);
  float getSoundVolume();
  float getMusicVolume();
  Key getKeyBind(Binding::Action action);

private:
  bool changed;
  float soundVolume;
  float musicVolume;

  void saveConfirmationUpdate();
  void saveAndExit();
  void cancelAndExit();
  void exitSettings();
};

