#pragma once
#include "Keys.h"
#include "Binding.h"
#include "GameLogic.h"
#include "InterfaceLogic.h"
#include "LayoutObject.h"

class Control
{
public:
  Control();
  ~Control();

  void keyDown(Key key);
  void keyUp(Key key);
  void mouseMove(float x, float y);
  void mouseDown(Key key);
  void mouseUp(Key key);
  void mouseScroll(float dx, float dy);
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

  const uint64_t freq;
  const uint64_t repeatDelay;
  const uint64_t repeatInterval;
  uint64_t currentCounter;
  KeyInternalState internalKeyStates[KEY_COUNT];
  double lastLButtonClickTimer;
  glm::vec2 lastLButtonClickPos;
  float mouseX;
  float mouseY;
  bool mouseMoved;
  bool mouseDoubleClicked;
  LayoutObjectId draggedProgressBarId;

  void updateGameControl();
  void updateMenuControl(MenuLogic & menu, LayoutObjectId layoutObjectId);
  void updateSettingsControl();
  void updateSettingsKeyBindControl();
  void updateLeaderboardControl();
  void updateInternalState();
  KeyState getKeyState(Key key) const;
};
