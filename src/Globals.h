#pragma once
#include "glall.h"

class Globals
{
private:
  Globals();
  ~Globals();

public:
  static std::string glErrorMessage;
  static GLuint mainArrayTextureId;
  static int mainArrayTextureSize;
  static int backgroundTexIndex;
  static int shadowTexIndex;
  static int openedBlocksTexIndex;
  static int closedBlocksTexIndex;
  static int vertBlocksTexIndex;
  static int horzBlocksTexIndex;

};

extern bool checkGlErrors();

