#include "static_headers.h"

#include "Application.h"

int Application::createCounter = 0;

Application::Application(GameLogic & model) : 
  model(model)
{
  ++createCounter;
}


Application::~Application()
{
  --createCounter;
}
