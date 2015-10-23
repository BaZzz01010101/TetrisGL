#include "Globals.h"

glm::vec3 Globals::ColorValues[7] =
{
  { 1.00f, 0.05f, 0.05f },
  { 1.00f, 0.35f, 0.00f },
  { 0.85f, 0.60f, 0.00f },
  { 0.05f, 0.55f, 0.10f },
  { 0.05f, 0.60f, 1.00f },
  { 0.10f, 0.20f, 0.90f },
  { 0.40f, 0.10f, 0.80f }
};

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
int Globals::dropTrailsTexIndex = 7;
int Globals::dropSparkleTexIndex = 8;
glm::vec2 Globals::gameBkPos(-0.7f, 1.0f);
glm::vec2 Globals::gameBkSize(1.4f, 2.0f);
glm::vec2 Globals::glassPos(-0.4f, 0.75f);
glm::vec2 Globals::glassSize(0.8f, 1.6f);
float Globals::holdNextTitleHeight = 0.05f;
float Globals::holdNextBkHorzGap = 0.04f;
float Globals::holdNextBkSize = 0.2f;
float Globals::scoreBarGap = 0.001f;
float Globals::scoreBarHeight = 0.04f;
float Globals::scoreBarScoreWidth = 0.35f;
float Globals::scoreBarMenuWidth = 0.25f;
float Globals::scoreBarValueWidth = Globals::gameBkSize.x - Globals::scoreBarScoreWidth - Globals::scoreBarMenuWidth;

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

double getTimer()
{
  static double freq = (double)Crosy::getPerformanceFrequency();
  assert(freq);

  return double(Crosy::getPerformanceCounter()) / freq;
}
