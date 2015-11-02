#include "static_headers.h"

#include "Crosy.h"
#include "GameLogic.h"
#include "OpenGlApplication.h"

#pragma warning(disable : 4100)

int main()
{
  srand((unsigned int)Crosy::getPerformanceCounter());

  GameLogic model;
  std::unique_ptr<Application> application = std::unique_ptr<OpenGLApplication>(new OpenGLApplication(model));

  if(!application->init())
    return 1;

  application->run();
  application->quit();

  return 0;
}
