#pragma once

#include "MenuLogic.h"
#include "Binding.h"

class SettingsLogic
{
public:
  typedef uint32_t Checksum;
  enum State { stHidden, stShowing, stVisible, stSaveConfirmation, stKeyWaiting, stHiding };
  enum Result { resNone, resClose };
  enum Control { ctrlNone, ctrlSoundVolume, ctrlMusicVolume, ctrlKeyBindTable, ctrlBackButton };

#pragma pack(push, 1)

  struct SaveData
  {
    float soundVolume;
    float musicVolume;
    Key actionKeys[Binding::ACTION_COUNT];
    Checksum dummy;
    Checksum checksum;
  } saveData;

#pragma pack(pop)

  MenuLogic saveConfirmationMenu;
  State state;
  const float showingTime;
  const float hidingTime;
  float transitionProgress;
  Control selectedControl;
  Control highlightedControl;
  Binding::Action selectedAction;
  Binding::Action highlightedAction;
  bool backButtonHighlighted;

  SettingsLogic();
  ~SettingsLogic();

  void init();
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
  std::string fileName;
  bool changed;
  float soundVolume;
  float musicVolume;

  void saveConfirmationUpdate();
  void saveAndExit();
  void cancelAndExit();
  void exitSettings();
};

