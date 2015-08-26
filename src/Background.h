#pragma once
#include "glall.h"
#include "Program.h"
#include "Shader.h"

class Background
{
private:
  GLuint vertexBufferId;
  GLuint uvBufferId;
  Program prog;
  Shader vert, frag;

public:
  const float aspect;
  Background();
  ~Background();

  void init();
  void setScreen(const glm::vec2 & screen);
  void draw() const;
};

