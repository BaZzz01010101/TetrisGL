#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "static_headers.h"

#include "Crosy.h"
#include "Logic.h"
#include "OpenGlApplication.h"

int main()
{
  int exitCode = 0;

  srand((unsigned int)Crosy::getPerformanceCounter());
  Binding::init();
  Logic::init();
  Application * application = new OpenGLApplication();

  if (application->init())
  {
    application->run();
    application->quit();
  }
  else
  {
#ifdef _DEBUG
    std::cout << "Press 'Enter'\n";
    getchar();
#endif // DEBUG

    exitCode = 1;
  }

  delete application;

  return exitCode;
}
