#pragma once

enum Action
{
  none,
  moveLeft, 
  moveRight, 
  rotateLeft, 
  rotateRight, 
  fastDown, 
  dropDown, 
  swapHold, 
  ACTION_COUNT
};

class Actions
{
private:
  Actions();
  ~Actions();
  static const char * const names[ACTION_COUNT];

public:
  static const char * getName(Action action);
};

