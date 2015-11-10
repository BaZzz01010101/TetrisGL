#pragma once
#include "GameLogic.h"
#include "InterfaceLogic.h"

class Application
{
protected:
  static int createCounter;
  GameLogic & gameLogic;
  InterfaceLogic & interfaceLogic;

public:
  Application(GameLogic & gameLogic, InterfaceLogic & interfaceLogic);
  virtual ~Application();

  virtual bool init() = 0;
  virtual void run() = 0;
  virtual void quit() = 0;
};

