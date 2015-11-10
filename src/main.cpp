#include "static_headers.h"

#include "Crosy.h"
#include "GameLogic.h"
#include "InterfaceLogic.h"
#include "OpenGlApplication.h"

#pragma warning(disable : 4100)

int main()
{
  srand((unsigned int)Crosy::getPerformanceCounter());

  GameLogic gameLogic;
  InterfaceLogic interfaceLogic;
  std::unique_ptr<Application> application = std::unique_ptr<OpenGLApplication>(new OpenGLApplication(gameLogic, interfaceLogic));

  if(!application->init())
    return 1;

  application->run();
  application->quit();

  return 0;
}
