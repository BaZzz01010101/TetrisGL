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
  static int backgroundTexIndex;
  static int shadowTexIndex;
  static int openBlocksTexIndex;
  static int closedBlocksTexIndex;
  static int vertBlocksTexIndex;
  static int horzBlocksTexIndex;
  static int dropTrailsTexIndex;
  static int dropSparklesTexIndex;
  static int blockTemplateTexIndex;
  static int boldBlockTemplateTexIndex;
  static int emptyTexIndex;
  static int holdFigureBkTexIndex;
  static int nextFigureBkTexIndex;
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

