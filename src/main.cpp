#include "static_headers.h"

#include "Crosy.h"
#include "GameLogic.h"
#include "InterfaceLogic.h"
#include "OpenGlApplication.h"

#pragma warning(disable : 4100)

int main()
{
  srand((unsigned int)Crosy::getPerformanceCounter());

  InterfaceLogic::init();
  GameLogic::init();
  std::unique_ptr<Application> application = std::make_unique<OpenGLApplication>();

  if(!application->init())
    return 1;

  application->run();
  application->quit();

  return 0;
}
