#pragma once
#include "glall.h"

class Block
{
private:
public:
  enum TYPE {btEmpty, btLeftUpRight, btLeftRight, btLeftUp, btLeftUpDot, btLeftDotDot, btFullSquare};
  glm::vec3 color;
  TYPE type;
  float rotation;

  Block();
  ~Block();
};

