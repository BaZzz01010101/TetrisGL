#pragma once
#include "GameLogic.h"
#include "InterfaceLogic.h"

class Logic
{
public:
  enum Result { resNone, resExitApp };
  static Result result;

  static void init();
  static void update();

private:
  Logic();
  ~Logic();
};

