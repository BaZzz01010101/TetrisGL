#pragma once
#include "glall.h"
#include <iostream>

class Globals
{
private:
  Globals();
  ~Globals();

public:
  enum Color { clNone = -1, clRed, clOrange, clYellow, clGreen, clCyan, clBlue, clPurple };
  static glm::vec3 Globals::ColorValues[7];
  static std::string glErrorMessage;
  static GLuint mainArrayTextureId;
  static int mainArrayTextureSize;
  static float mainArrayTexturePixelSize;

  static int emptyTexIndex;
  static int backgroundTexIndex;
  static int blockTemplateTexIndex;
  static int boldBlockTemplateTexIndex;
  static int shadowTexIndex;
  static int holdFigureBkTexIndex;
  static int nextFigureBkTexIndex;
  static int dropTrailTexIndex;
  static int dropSparkleTexIndex;
  static int rowFlashTexIndex;
  static int rowShineRayTexIndex;
  static int rowShineLightTexIndex;
  static const int nextFiguresCount = 3;
  static glm::vec2 gameBkPos;
  static glm::vec2 gameBkSize;
  static glm::vec2 glassPos;
  static glm::vec2 glassSize;
  static float holdNextTitleHeight;
  static float holdNextBkHorzGap;
  static float holdNextBkSize;
  static float scoreBarGap;
  static float scoreBarHeight;
  static float scoreBarScoreWidth;
  static float scoreBarValueWidth;
  static float scoreBarMenuWidth;
};

extern bool checkGlErrors();
extern double getTimer();

static int g_seed = rand();
static const int FAST_RAND_MAX = 0x7FFF;
inline int fastrand()
{
  g_seed = (214013 * g_seed + 2531011);
  return (g_seed >> 16) & FAST_RAND_MAX;
}

