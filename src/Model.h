#pragma once
#include <vector>
#include <list>

#include "Cell.h"
#include "Figure.h"

class Model
{
private:
  const int nextFiguresCount;
  const int maxLevel;
  const float maxStepTime;
  const float minStepTime;
  float lastStepTime;

  void initGame(int glassWidth, int glassHeight);
  void pulse();
  float getTime();
  void shiftFigureConveyor();
  bool checkCurrentFigurePos(int dx, int dy);
  bool tryToRelocateCurrentFigure();

public:
  enum GameState { gsStartGame, gsPlayingGame, gsGameOver };
  GameState gameState;
  std::vector<Cell> glass;
  std::list<Figure> nextFigures;
  Figure * curFigure;
  int glassWidth;
  int glassHeight;
  int curFigureX;
  int curFigureY;
  int curLevel;
  bool glassChanged;
  bool nextFiguresChanged;

  Model();
  ~Model();

  void update();

  void dropCurrentFigure();
  void rotateCurrentFigureLeft();
  void rotateCurrentFigureRight();
  void shiftCurrentFigureLeft();
  void shiftCurrentFigureRight();
  void storeCurFigureToGlass();
  void cleanCurFigureFromGlass();
};

