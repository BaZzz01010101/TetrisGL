#pragma once

class SettingsLogic
{
public:
  enum State { stHidden, stShowing, stVisible, stSaveConfirmation, stHiding };
  enum Result { resNone, resSaveConfirmation, resClose };

  ReadOnly<State, SettingsLogic> state;
  ReadOnly<Result, SettingsLogic> result;
  ReadOnly<float, SettingsLogic> transitionProgress;

  SettingsLogic();
  ~SettingsLogic();

  Result update();
  void escape();

private:
  bool changed;
};

