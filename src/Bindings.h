#pragma once

#include "Keys.h"

class Bindings
{
public:
  enum Action { doNothing = -1, moveLeft, moveRight, rotateLeft, rotateRight, fastDown, dropDown, swapHold, ACTION_COUNT };

  Bindings();
  ~Bindings();

  void setDefault();
  void setKeyBinding(Key key, Action action);
  Action getKeyAction(Key key) const;
  Key getActionKey(Action action) const;
  const char * getActionName(Action action);

private:
  static const char * const actionNames[ACTION_COUNT];
  Action keyActions[KEY_COUNT];
};

