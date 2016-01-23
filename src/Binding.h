#pragma once

#include "Keys.h"

class Binding
{
public:
  enum Action 
  { 
    doNothing = -1, 
    FIRST_ACTION = 0, 
    moveLeft = 0, 
    moveRight, 
    rotateLeft, 
    rotateRight, 
    fastDown, 
    dropDown, 
    swapHold, 
    ACTION_COUNT 
  };

  static void init();
  static void setKeyBinding(Key key, Action action);
  static Action getKeyAction(Key key);
  static Key getActionKey(Action action);
  static const char * getActionName(Action action);

private:
  Binding();
  ~Binding();

  static const char * const actionNames[ACTION_COUNT];
  static Key actionKeys[ACTION_COUNT];
};

Binding::Action operator++(Binding::Action & action);
Binding::Action operator++(Binding::Action & action, int);
Binding::Action operator--(Binding::Action & action);
Binding::Action operator--(Binding::Action & action, int);
Binding::Action operator+(Binding::Action action, int value);
Binding::Action operator-(Binding::Action action, int value);
