#pragma once

#include "Keys.h"

class Binding
{
public:
  enum Action { doNothing = -1, moveLeft, moveRight, rotateLeft, rotateRight, fastDown, dropDown, swapHold, ACTION_COUNT };

  static void init();
  static void setKeyBinding(Key key, Action action);
  static Action getKeyAction(Key key);
  static Key getActionKey(Action action);
  static const char * getActionName(Action action);

private:
  Binding();
  ~Binding();

  static const char * const actionNames[Action::ACTION_COUNT];
  static Action keyActions[Key::KEY_COUNT];
};

