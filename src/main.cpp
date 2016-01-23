#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "static_headers.h"

#include "Crosy.h"
#include "Logic.h"
#include "OpenGlApplication.h"

int main()
{
  srand((unsigned int)Crosy::getPerformanceCounter());
  Binding::init();
  Logic::init();
  Application * application = new OpenGLApplication();

  if (!application->init())
  {
#ifdef _DEBUG
    std::cout << "Press 'Enter'\n";
    getchar();
#endif // DEBUG
    return 1;
  }

  application->run();
  application->quit();

  delete application;

  return 0;
}
