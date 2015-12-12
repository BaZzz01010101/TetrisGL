#pragma once

#include "Binding.h"

class SettingsLogic
{
public:
  enum State { stHidden, stShowing, stVisible, stSaveConfirmation, stHiding };
  enum Result { resNone, resSaveConfirmation, resClose };
  enum Control { ctrlSoundVolume, ctrlMusicVolume, ctrlKeyBindTable };

  State state;
  Result result;
  float transitionProgress;
  float soundVolume;
  float musicVolume;
  Control selectedControl;
  Binding::Action selectedAction;

  SettingsLogic();
  ~SettingsLogic();

  Result update();
  void escape();

private:
  bool changed;
};

