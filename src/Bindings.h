#pragma once

#include "Actions.h"
#include "Keys.h"

class Bindings
{
private:
  Action keyActions[KEY_COUNT];

public:
  Bindings();
  ~Bindings();

  void setDefault();
  void setKeyBinding(Key key, Action action);
  Action getKeyAction(Key key);
  Key getActionKey(Action action);
};

