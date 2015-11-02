#include "static_headers.h"

#include "Control.h"
#include "Crosy.h"


Control::Control(GameLogic & gameLogic) : 
  gameLogic(gameLogic),
  freq(Crosy::getPerformanceFrequency()),
  repeatDelay(uint64_t(0.2 * freq)),
  repeatInterval(uint64_t(0.05 * freq))
{
  assert(freq);
  memset(keyNextRepeatCounters, 0, sizeof(keyNextRepeatCounters));
  memset(lastKeyStates, 0, sizeof(lastKeyStates));
}


Control::~Control()
{
}

uint64_t Control::getNextRepeatCounter(Key key)
{
  uint64_t currentCounter = Crosy::getPerformanceCounter();
  uint64_t keyNextRepeatCounter = keyNextRepeatCounters[key];

  if (currentCounter <= keyNextRepeatCounter)
    return keyNextRepeatCounter;
  else
    return keyNextRepeatCounter + ((currentCounter - keyNextRepeatCounter) / repeatInterval + 1) * repeatInterval;
}

void Control::keyDown(Key key)
{
  keyNextRepeatCounters[key] = Crosy::getPerformanceCounter() + repeatDelay;
}

void Control::keyUp(Key key)
{
  keyNextRepeatCounters[key] = 0;
}

void Control::update()
{
  processKeyboard();
}

void Control::processKeyboard()
{
  for (int i = 0; i < KEY_COUNT; i++)
  {
    Key key = Key(i);
    Action action = gameLogic.bindings.getKeyAction(key);

    if (action)
    {
      uint64_t currentCounter = Crosy::getPerformanceCounter();
      uint64_t keyNextRepeatCounter = keyNextRepeatCounters[key];
      bool lastKeyState = lastKeyStates[key];
      bool keyState = (keyNextRepeatCounter > 0);
      bool timeToRepeat = (keyState && currentCounter > keyNextRepeatCounters[key]);
      bool keyPressed = (keyState && !lastKeyState) || timeToRepeat;
      lastKeyStates[key] = keyState;

      if (action == fastDown)
        gameLogic.fastDown = keyState;

      if (keyPressed) 
        switch (action)
      {
        case moveLeft:    gameLogic.shiftCurrentFigureLeft();   break;
        case moveRight:   gameLogic.shiftCurrentFigureRight();  break;
        case rotateLeft:  gameLogic.rotateCurrentFigureLeft();  break;
        case rotateRight: gameLogic.rotateCurrentFigureRight(); break;
        default: break;
      }

      if (keyState && !lastKeyState) 
        switch (action)
      {
        case dropDown: gameLogic.dropCurrentFigure(); break;
        case swapHold: gameLogic.holdCurrentFigure(); break;
        default: break;
      }
      

    }
  }
}
