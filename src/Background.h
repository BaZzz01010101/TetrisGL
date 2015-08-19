#pragma once
#include "glall.h"
#include "Program.h"
#include "Shader.h"

class Background
{
private:
  GLuint vertexBufferId;
  GLuint uvBufferId;
  GLuint textureId;
  Program prog;
  Shader vert, frag;
  glm::vec2 screenScale;

public:
  const float aspect;
  Background();
  ~Background();

  void init();
  void setScreenScale(const glm::vec2 & scale);
  void draw() const;
};

