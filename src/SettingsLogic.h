#pragma once

#include "MenuLogic.h"
#include "Binding.h"

class SettingsLogic
{
public:
  enum State { stHidden, stShowing, stVisible, stSaveConfirmation, stKeyWaiting, stHiding };
  enum Result { resNone, resClose };
  enum Control { ctrlNone, ctrlSoundVolume, ctrlMusicVolume, ctrlKeyBindTable, ctrlBackButton };

#pragma pack(push, 1)

  struct SaveData
  {
    float soundVolume;
    float musicVolume;
    Key actionKeys[Binding::ACTION_COUNT];
    uint32_t dummy;
    uint32_t checksum;
  } saveData;

#pragma pack(pop)

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
  void setCurrentActionKey(Key key);
  float getSoundVolume();
  float getMusicVolume();
  Key getKeyBind(Binding::Action action);
  void selectNext();
  void selectPrevious();

private:
  static const char * fileName;
  bool changed;
  float soundVolume;
  float musicVolume;

  void saveConfirmationUpdate();
  void saveAndExit();
  void cancelAndExit();
  void exitSettings();
};

