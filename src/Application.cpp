#include "static_headers.h"

#include "Application.h"

int Application::createCounter = 0;

Application::Application(GameLogic & gameLogic, InterfaceLogic & interfaceLogic) :
  gameLogic(gameLogic),
  interfaceLogic(interfaceLogic)
{
  assert(createCounter == 0);
  ++createCounter;
}


Application::~Application()
{
  assert(createCounter == 1);
  --createCounter;
}
