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
  glm::mat3x3 proj;

public:
  const float aspect;
  Background();
  ~Background();

  void init();
  void setProjMatrix(const glm::mat3x3 & m);
  void draw() const;
};

