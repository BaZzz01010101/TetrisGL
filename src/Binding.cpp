#include "static_headers.h"

#include "Binding.h"

const char * const Binding::actionNames[Binding::ACTION_COUNT] =
{
  "Move Left",
  "Move Right",
  "Rotate Left",
  "Rotate Right",
  "Fast Down",
  "Drop",
  "Hold",
};

Binding::Action Binding::keyActions[Key::KEY_COUNT];

Binding::Binding()
{
  init();
}


Binding::~Binding()
{
}

void Binding::init()
{
  std::fill_n(keyActions, (int)Key::KEY_COUNT, doNothing);

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

Binding::Action Binding::getKeyAction(Key key) 
{
  return keyActions[key];
}

Key Binding::getActionKey(Action action) 
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
