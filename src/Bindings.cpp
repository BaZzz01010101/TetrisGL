#include "static_headers.h"

#include "Bindings.h"

Bindings::Bindings()
{
  setDefault();
}


Bindings::~Bindings()
{
}

void Bindings::setDefault()
{
  keyActions[KB_LEFT] = moveLeft;
  keyActions[KB_RIGHT] = moveRight;
  keyActions[KB_UP] = rotateLeft;
  keyActions[KB_DOWN] = rotateRight;
  keyActions[KB_ENTER] = fastDown;
  keyActions[KB_SPACE] = dropDown;
  keyActions[KB_RIGHT_CONTROL] = swapHold;
}

void Bindings::setKeyBinding(Key key, Action action)
{
  keyActions[key] = action;
}

Action Bindings::getKeyAction(Key key)
{
  return keyActions[key];
}

Key Bindings::getActionKey(Action action)
{
  for (int key = 0; key < KEY_COUNT; key++)
  if (keyActions[key] == action)
    return Key(key);

  return KB_NONE;
}

