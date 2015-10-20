#pragma once
#include <vector>
#include <list>

#include "Cell.h"
#include "Figure.h"

class Model
{
private:
  const int maxLevel;
  const float maxStepTime;
  const float minStepTime;
  float lastStepTime;

  void initGame(int glassWidth, int glassHeight);
  void pulse();
  float getTime();
  float getStepTime();
  void shiftFigureConveyor();
  bool checkCurrentFigurePos(int dx, int dy);
  bool tryToRelocateCurrentFigure();
  void checkGlassRows();

public:
  enum GameState { gsStartGame, gsPlayingGame, gsGameOver };
  GameState gameState;
  std::vector<Cell> glass;
  Figure nextFigures[Globals::nextFiguresCount];
  Figure holdFigure;
  Figure curFigure;
  int glassWidth;
  int glassHeight;
  int curFigureX;
  int curFigureY;
  int curLevel;
  bool forceDown;
  bool haveHold;
  bool justHolded;
  bool glassChanged;
  bool nextFiguresChanged;

  Model();
  ~Model();

  void update();

  void holdCurrentFigure();
  void dropCurrentFigure();
  void rotateCurrentFigureLeft();
  void rotateCurrentFigureRight();
  void shiftCurrentFigureLeft();
  void shiftCurrentFigureRight();
  void storeCurFigureIntoGlass();
};

