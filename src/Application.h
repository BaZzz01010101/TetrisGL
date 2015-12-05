#pragma once
#include "GameLogic.h"

class Application
{
protected:
  static int createCounter;

public:
  Application();
  virtual ~Application();

  virtual bool init() = 0;
  virtual void run() = 0;
  virtual void quit() = 0;
};

