#include "static_headers.h"

#include "Layout.h"
#include "Globals.h"
#include "Crosy.h"

std::string Globals::glErrorMessage;
GLuint Globals::mainArrayTextureId = 0;
int Globals::mainArrayTextureSize = 64;
float Globals::mainArrayTexturePixelSize = 1.0f / Globals::mainArrayTextureSize;
int Globals::emptyTexIndex = 0;
int Globals::backgroundTexIndex = 1;
int Globals::blockTemplateTexIndex = 2;
int Globals::boldBlockTemplateTexIndex = 3;
int Globals::shadowTexIndex = 4;
int Globals::holdFigureBkTexIndex = 5;
int Globals::nextFigureBkTexIndex = 6;
int Globals::dropTrailTexIndex = 7;
int Globals::dropSparkleTexIndex = 8;
int Globals::rowFlashTexIndex = 9;
int Globals::rowShineRayTexIndex = 10;
int Globals::rowShineLightTexIndex = 11;
int Globals::sidePanelGlowTexIndex = 12;
int Globals::lineTexIndex = 13;
int Globals::levelGoalBkTexIndex = 14;
int Globals::levelBackShevronTexIndex = 15;
int Globals::fontFirstTexIndex = 16;

int Globals::nextFiguresCount = 3;
float Globals::rowsDeletionEffectTime = 0.8f;
float Globals::menuShowingTime = 0.4f;
float Globals::menuHidingTime = 0.3f;
float Globals::settingsShowingTime = 0.3f;
float Globals::settingsHidingTime = 0.2f;
int Globals::smallFontSize = 24;
int Globals::midFontSize = 32;
int Globals::bigFontSize = 48;

Globals::Globals()
{
}


Globals::~Globals()
{
}

bool checkGlErrors()
{
  GLenum errCode = glGetError();

  if (errCode)
  {
    switch (errCode)
    {

    case GL_NO_ERROR:
      Globals::glErrorMessage = "No error";
      break;
    case GL_INVALID_ENUM:
      Globals::glErrorMessage = "Invalid enum";
      break;
    case GL_INVALID_VALUE:
      Globals::glErrorMessage = "Invalid value";
      break;
    case GL_INVALID_OPERATION:
      Globals::glErrorMessage = "Invalid operation";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      Globals::glErrorMessage = "Invalid framebuffer operation";
      break;
    case GL_OUT_OF_MEMORY:
      Globals::glErrorMessage = "Out of memory";
      break;
    case GL_STACK_UNDERFLOW:
      Globals::glErrorMessage = "Stack underflow";
      break;
    case GL_STACK_OVERFLOW:
      Globals::glErrorMessage = "Stack overflow";
      break;
    default:
      Globals::glErrorMessage = "Unknown error";
    }

    std::cout << Globals::glErrorMessage << "\n";
  }
  else
    Globals::glErrorMessage.clear();

  return errCode != 0;
}
