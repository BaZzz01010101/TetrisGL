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
int Globals::sidePanelGlowTexIndex = 12;
int Globals::lineTexIndex = 13;
int Globals::levelGoalBkTexIndex = 14;
int Globals::levelBackShevronTexIndex = 15;
int Globals::fontFirstTexIndex = 16;

glm::vec2 Globals::bkTileSize(0.03f, 0.02f);
glm::vec2 Globals::sidePanelBkTileSize(0.024f, 0.016f);
int Globals::nextFiguresCount = 3;
float Globals::rowsDeletionEffectTime = 0.8f;
glm::vec2 Globals::gameBkPos(-0.75f, 1.0f);
glm::vec2 Globals::gameBkSize(1.5f, 2.0f);
glm::vec2 Globals::glassPos(-0.45f, 0.85f);
glm::vec2 Globals::glassSize(0.9f, 1.8f);
float Globals::defaultCaptionHeight = 0.05f;
float Globals::holdNextBkHorzGap = 0.04f;
float Globals::holdNextBkSize = 0.2f;
float Globals::scoreBarGaps = 0.01f;
float Globals::scoreBarHeight = 0.1f;
float Globals::scoreBarCaptionWidth = 0.45f;
float Globals::scoreBarMenuWidth = 0.25f;
float Globals::scoreBarValueWidth = Globals::gameBkSize.x - Globals::scoreBarCaptionWidth - Globals::scoreBarMenuWidth;
float Globals::menuTop = 0.5f;
float Globals::menuRowWidth = 0.5f * Globals::gameBkSize.x;
float Globals::menuRowHeight = 0.06f * Globals::gameBkSize.y;
float Globals::menuRowInterval = 0.04f * Globals::gameBkSize.y;
float Globals::menuRowGlowWidth = 0.03f;
float Globals::menuRowCornerSize = 0.03f;
float Globals::menuShowingTime = 0.4f;
float Globals::menuHidingTime = 0.3f;
glm::vec3 Globals::menuNormalPanelColor(0.4f, 0.7f, 1.2f);
glm::vec3 Globals::menuSelectedPanelColor(2.0f, 1.9f, 1.2f);
glm::vec3 Globals::menuNormalTextColor(0.4f, 0.7f, 1.0f);
glm::vec3 Globals::menuSelectedTextColor(1.0f, 0.9f, 0.4f);
float Globals::settingsTop = 0.5f;
float Globals::settingsWidth = 0.75f * Globals::gameBkSize.x;
float Globals::settingsHeight = 0.5f * Globals::gameBkSize.y;
float Globals::settingsGlowWidth = 0.05f;
float Globals::settingsCornerSize = 0.08f;
float Globals::settingsShowingTime = 0.4f;
float Globals::settingsHidingTime = 0.3f;
glm::vec3 Globals::settingsTopBkColor(0.4f, 0.7f, 1.2f);
glm::vec3 Globals::settingsBottomBkColor(0.4f, 0.7f, 1.2f);
glm::vec3 Globals::settingsPanelTopBkColor = 0.2f * settingsTopBkColor;
glm::vec3 Globals::settingsPanelBottomBkColor = 0.25f * Globals::settingsPanelTopBkColor;
glm::vec3 Globals::settingsPanelActiveRowBkColor(0.75f);
glm::vec3 Globals::settingsPanelInactiveRowBkColor(0.25f);
glm::vec3 Globals::settingsPanelActiveRowFgColor(0.25f);
glm::vec3 Globals::settingsPanelInactiveRowFgColor(0.75f);
glm::vec3 Globals::settingsCaptionColor(1.0f, 0.9f, 0.4f);
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
