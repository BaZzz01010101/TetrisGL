#include "static_headers.h"

#include "Application.h"

int Application::createCounter = 0;

Application::Application()
{
  assert(createCounter == 0);
  ++createCounter;
}


Application::~Application()
{
  assert(createCounter == 1);
  --createCounter;
}
