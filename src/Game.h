#pragma once
#include "Background.h"
#include "Shader.h"
#include "Program.h"

class Game
{
private:
  GLuint vertexArrayId;
  Background background;
  Shader blockVS, blockFS;
  Program blockProg;
  const float width;
  const float height;

public:
  Game();
  ~Game();

  bool init();
  void resize(float aspect);
  void pulse();
  void onKeyClick(int key, int scancode, int action, int mods);
  void onMouseClick(int key, int action, int mods);
  void onMouseMove(double xpos, double ypos);

  void drawBackground();
  void drawGlass();
};

