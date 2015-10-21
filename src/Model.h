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
  double lastStepTimer;

  void initGame(int glassWidth, int glassHeight);
  void pulse();
  float getStepTime();
  void shiftFigureConveyor();
  bool checkCurrentFigurePos(int dx, int dy);
  bool tryToPlaceCurrentFigure();
  void checkGlassRows();
  void proceedFallingRows();
  double getTimer();

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
  double startFallTimer;

  Model();
  ~Model();

  void update();

  float getCellCurrentElevation(const Cell & cell);
  void holdCurrentFigure();
  void dropCurrentFigure();
  void rotateCurrentFigureLeft();
  void rotateCurrentFigureRight();
  void shiftCurrentFigureLeft();
  void shiftCurrentFigureRight();
  void storeCurFigureIntoGlass();
};

