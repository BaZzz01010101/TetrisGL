#include "static_headers.h"

#include "Globals.h"
#include "Crosy.h"

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
int Globals::dropTrailTexIndex = 7;
int Globals::dropSparkleTexIndex = 8;
int Globals::rowFlashTexIndex = 9;
int Globals::rowShineRayTexIndex = 10;
int Globals::rowShineLightTexIndex = 11;
int Globals::sidePanelGlowingBorderTexIndex = 12;
int Globals::sidePanelInnerGlowTexIndex = 13;
int Globals::levelGoalBkTexIndex = 14;
int Globals::fontFirstTexIndex = 15;
glm::vec2 Globals::gameBkPos(-0.75f, 1.0f);
glm::vec2 Globals::gameBkSize(1.5f, 2.0f);
glm::vec2 Globals::glassPos(-0.45f, 0.85f);
glm::vec2 Globals::glassSize(0.9f, 1.8f);
float Globals::dafaultCaptionHeight = 0.05f;
float Globals::holdNextBkHorzGap = 0.04f;
float Globals::holdNextBkSize = 0.2f;
float Globals::scoreBarGaps = 0.01f;
float Globals::scoreBarHeight = 0.1f;
float Globals::scoreBarCaptionWidth = 0.45f;
float Globals::scoreBarMenuWidth = 0.25f;
float Globals::scoreBarValueWidth = Globals::gameBkSize.x - Globals::scoreBarCaptionWidth - Globals::scoreBarMenuWidth;
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

double getTimer()
{
  static double freq = (double)Crosy::getPerformanceFrequency();
  assert(freq);

  if (freq > VERY_SMALL_NUMBER)
    return double(Crosy::getPerformanceCounter()) / freq;
  else
    return 0.0;
}
