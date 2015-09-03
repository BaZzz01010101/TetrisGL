#pragma once
#include "glall.h"
#include "Background.h"
#include "Shader.h"
#include "Program.h"
#include "Figure.h"
#include "DropTrail.h"

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
  std::list<DropTrail> dropTrails;
  Figure * curFigure;

  float getTime();
  void beforeStartGame();
  void nextFigure();
  bool validateRotation();
  bool checkCurrentFigurePos(int dx, int dy);
  void step();
  void drop();
  void cleanupDropTrails();

  void draw();

public:
  enum KeyMask { kmNone = 0, kmLeft = 0x01, kmRight = 0x02, kmRotRight = 0x04, kmRotLeft = 0x08, kmDown = 0x10, kmDrop = 0x20 };
  uint32_t keyState;

  Game();
  ~Game();

  bool init();
  void pulse();
  void onKeyClick(int key, int scancode, int action, int mods);
  void onMouseClick(int key, int action, int mods);
  void onMouseMove(double xpos, double ypos);
};

