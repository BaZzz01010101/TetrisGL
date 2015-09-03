#pragma once
#include <vector>
#include <list>

#include "Cell.h"
#include "Figure.h"

class Model
{
private:
  enum GameState { gsStartGame, gsPlayingGame, gsGameOver };
  GameState gameState;
  const int nextFiguresCount;
  const int maxLevel;
  const float maxStepTime;
  const float minStepTime;

  void initGame(int glassWidth, int glassHeight);
  void pulse();
  float getTime();
  void shiftFigureConveyor();
  bool checkCurrentFigurePos(int dx, int dy);
  bool tryToRelocateCurrentFigure();

public:
  std::vector<Cell> glass;
  std::list<Figure> nextFigures;
  Figure * curFigure;
  int glassWidth;
  int glassHeight;
  int curFigureX;
  int curFigureY;
  int curLevel;
  bool glassChanged;
  bool curFigureChanged;
  bool nextFiguresChanged;

  Model();
  ~Model();

  void update();

  void dropCurrentFigure();
  void rotateCurrentFigureLeft();
  void rotateCurrentFigureRight();
  void shiftCurrentFigureLeft();
  void shiftCurrentFigureRight();
};

