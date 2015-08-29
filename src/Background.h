#pragma once
#include "glall.h"
#include "Program.h"
#include "Shader.h"

class Background
{
private:
  GLuint vertexBufferId;
  GLuint uvBufferId;
  Program gameBkProg;
  Shader gameBkVert;
  Shader gameBkFrag;
  Program glassBkProg;
  Shader glassBkVert;
  Shader glassBkFrag;

public:
  Background();
  ~Background();

  void init();
  void draw() const;
};

