#include "static_headers.h"

#include "Bindings.h"

const char * const Bindings::actionNames[ACTION_COUNT] =
{
  "Move Left",
  "Move Right",
  "Rotate Left",
  "Rotate Right",
  "Fast Down",
  "Drop",
  "Hold",
};

Bindings::Bindings()
{
  setDefault();
}


Bindings::~Bindings()
{
}

void Bindings::setDefault()
{
  for (Key key = Key::KB_NONE; key < Key::KEY_COUNT; key++)
    keyActions[key] = doNothing;

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

Bindings::Action Bindings::getKeyAction(Key key) const
{
  return keyActions[key];
}

Key Bindings::getActionKey(Action action) const
{
  for (int key = 0; key < KEY_COUNT; key++)
    if (keyActions[key] == action)
      return Key(key);

  return KB_NONE;
}

const char * Bindings::getActionName(Action action)
{
  return actionNames[action];
}
