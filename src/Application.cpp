#include "static_headers.h"

#include "Application.h"

int Application::createCounter = 0;

Application::Application(GameLogic & gameLogic) : 
  gameLogic(gameLogic)
{
  ++createCounter;
}


Application::~Application()
{
  --createCounter;
}
