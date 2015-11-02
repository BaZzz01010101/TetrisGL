#pragma once
#include "Keys.h"
#include "Actions.h"
#include "GameLogic.h"

class Control
{
private:
  GameLogic & gameLogic;
//  uint64_t keyDownCounters[KEY_COUNT];
  uint64_t keyNextRepeatCounters[KEY_COUNT];
  bool lastKeyStates[KEY_COUNT];
  const uint64_t freq;
  const uint64_t repeatDelay;
  const uint64_t repeatInterval;

  uint64_t getNextRepeatCounter(Key key);
  void processKeyboard();

public:
  Control(GameLogic & gameLogic);
  ~Control();

  void keyDown(Key key);
  void keyUp(Key key);
  void update();
};

