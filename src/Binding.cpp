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

Key Binding::actionKeys[Action::ACTION_COUNT];

Binding::Binding()
{
  init();
}


Binding::~Binding()
{
}

void Binding::init()
{
  std::fill_n(actionKeys, (int)Action::ACTION_COUNT, Key::KB_NONE);

  actionKeys[moveLeft] = KB_LEFT;
  actionKeys[moveRight] = KB_RIGHT;
  actionKeys[rotateLeft] = KB_UP;
  actionKeys[rotateRight] = KB_DOWN;
  actionKeys[fastDown] = KB_ENTER;
  actionKeys[dropDown] = KB_SPACE;
  actionKeys[swapHold] = KB_RIGHT_CONTROL;
}

void Binding::setKeyBinding(Key key, Action action)
{
  for (Binding::Action a = doNothing + 1; a < ACTION_COUNT; a++)
    if (actionKeys[a] == key)
      actionKeys[a] = KB_NONE;

  actionKeys[action] = key;
}

Binding::Action Binding::getKeyAction(Key key) 
{
  for (Binding::Action action = doNothing + 1; action < ACTION_COUNT; action++)
    if (actionKeys[action] == key)
      return action;

  return doNothing;
}

Key Binding::getActionKey(Action action) 
{
  return actionKeys[action];
}

const char * Binding::getActionName(Action action)
{
  return actionNames[action];
}

Binding::Action operator++(Binding::Action & action)
{
  action = action + 1;

  return action;
}

Binding::Action operator++(Binding::Action & action, int)
{
  Binding::Action prevAction = action;
  action = action + 1;

  return prevAction;
}

Binding::Action operator--(Binding::Action & action)
{
  action = action - 1;

  return action;
}

Binding::Action operator--(Binding::Action & action, int)
{
  Binding::Action prevAction = action;
  action = action - 1;

  return prevAction;
}

Binding::Action operator+(Binding::Action action, int value)
{
  action = static_cast<Binding::Action>(static_cast<int>(action) + value);

  if (action > Binding::ACTION_COUNT)
    action = Binding::ACTION_COUNT;

  return action;
}

Binding::Action operator-(Binding::Action action, int value)
{
  action = static_cast<Binding::Action>(static_cast<int>(action)-value);

  if (action < Binding::doNothing)
    action = Binding::doNothing;

  return action;
}
