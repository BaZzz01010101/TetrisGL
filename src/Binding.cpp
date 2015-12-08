#include "static_headers.h"

#include "Binding.h"

const char * const Binding::actionNames[ACTION_COUNT] =
{
  "Move Left",
  "Move Right",
  "Rotate Left",
  "Rotate Right",
  "Fast Down",
  "Drop",
  "Hold",
};

Binding::Binding()
{
  setDefault();
}


Binding::~Binding()
{
}

void Binding::setDefault()
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

void Binding::setKeyBinding(Key key, Action action)
{
  keyActions[key] = action;
}

Binding::Action Binding::getKeyAction(Key key) const
{
  return keyActions[key];
}

Key Binding::getActionKey(Action action) const
{
  for (int key = 0; key < KEY_COUNT; key++)
    if (keyActions[key] == action)
      return Key(key);

  return KB_NONE;
}

const char * Binding::getActionName(Action action)
{
  return actionNames[action];
}
