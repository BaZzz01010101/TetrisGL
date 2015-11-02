#include "Actions.h"

const char * const Actions::names[ACTION_COUNT] =
{
  "",
  "Move Left",
  "Move Right",
  "Rotate Left",
  "Rotate Right",
  "Fast Down",
  "Drop",
  "Hold",
};

const char * Actions::getName(Action action)
{
  return names[action];
}
