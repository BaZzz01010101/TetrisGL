#pragma once
#include "glall.h"
#include "Background.h"
#include "Shader.h"
#include "Program.h"
#include "Figure.h"

class Game
{
private:
  enum GameState { gsStartGame, gsPlayingGame, gsGameOver };
  GameState gameState;
  GLuint vertexArrayId;
  Background background;
  int glassWidth;
  int glassHeight;
  float glassSqSize;
  const int maxLevel;
  const float maxStepTime;
  const float minStepTime;
  int level; 
  int figx, figy;
  std::list<Figure> glassFallingFigures;
  std::list<Figure> glassFigures;
  std::list<Figure> nextFigures;
  Figure * curFigure;

  float getTime();
  void step();
  void drop();
  bool validateRotation();
  bool checkPos(int dx, int dy);
  void draw();

  void beforeStartGame();
  void nextFigure();

public:
  enum KeyMask { kmNone = 0, kmLeft = 0x01, kmRight = 0x02, kmRotRight = 0x04, kmRotLeft = 0x08, kmDown = 0x10, kmDrop = 0x20 };
  uint32_t keyState;

  Game();
  ~Game();

  bool init();
  void resize(float aspect);
  void pulse();
  void onKeyClick(int key, int scancode, int action, int mods);
  void onMouseClick(int key, int action, int mods);
  void onMouseMove(double xpos, double ypos);
};

