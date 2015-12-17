#pragma once

#include "Binding.h"

class SettingsLogic
{
public:
  enum State { stHidden, stShowing, stVisible, stSaveConfirmation, stHiding };
  enum Result { resNone, resSaveConfirmation, resClose };
  enum Control { ctrlNone, ctrlSoundVolume, ctrlMusicVolume, ctrlKeyBindTable, ctrlBackButton };

  State state;
  Result result;
  float transitionProgress;
  float soundVolume;
  float musicVolume;
  Control selectedControl;
  Control highlightedControl;
  Binding::Action selectedAction;
  Binding::Action highlightedAction;
  bool backButtonHighlighted;


  SettingsLogic();
  ~SettingsLogic();

  Result update();
  void escape();

private:
  bool changed;
};

