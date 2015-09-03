#pragma once
#include "glall.h"
#include <iostream>

class Globals
{
private:
  Globals();
  ~Globals();

public:
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
  static glm::vec2 gameBkPos;
  static glm::vec2 gameBkSize;
  static glm::vec2 glassPos;
  static glm::vec2 glassSize;
};

extern bool checkGlErrors();

