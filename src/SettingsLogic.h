#pragma once

class SettingsLogic
{
public:
  enum State { stHidden, stShowing, stVisible, stSaveConfirmation, stHiding };
  enum Result { resNone, resSaveConfirmation, resClose };

  State state;
  Result result;
  float transitionProgress;

  SettingsLogic();
  ~SettingsLogic();

  Result update();
  void escape();

private:
  bool changed;
};

