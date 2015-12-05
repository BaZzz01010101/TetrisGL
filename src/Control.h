#pragma once
#include "Keys.h"
#include "Actions.h"
#include "GameLogic.h"
#include "InterfaceLogic.h"

class Control
{
public:
  Control();
  ~Control();

  void keyDown(Key key);
  void keyUp(Key key);
  void mouseMove(float x, float y);
  void mouseDown();
  void update();

private:
  struct KeyState
  {
    bool isPressed;
    bool wasChanged;
    int pressCount;
    int repeatCount;
  };

  struct KeyInternalState
  {
    uint64_t keyNextRepeatCounter; 
    int pressCount;
    bool wasChanged;
  };

  Bindings bindings;
  const uint64_t freq;
  const uint64_t repeatDelay;
  const uint64_t repeatInterval;
  uint64_t currentCounter;
  KeyInternalState internalKeyStates[KEY_COUNT];

  void updateKeyboard();
  void updateGameKeyboard();
  void updateMenuKeyboard(MenuLogic & menu);
  void updateSettingsKeyboard();
  void updateLeaderboardKeyboard();
  void updateKeyStates();
  KeyState getKeyState(Key key) const;

  float mouseX;
  float mouseY;
};
