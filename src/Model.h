#pragma once
#include <vector>
#include <list>

#include "Cell.h"
#include "Figure.h"
#include "DropSparkle.h"
#include "DropTrail.h"
#include "RowFlash.h"

class Model
{
private:
  const int maxLevel;
  const float maxStepTime;
  const float minStepTime;
  double lastStepTimer;

  void initGameProceed(int glassWidth, int glassHeight);
  void playingGameProceed();
  float getStepTime();
  void shiftFigureConveyor();
  bool checkCurrentFigurePos(int dx, int dy);
  bool tryToPlaceCurrentFigure();
  void checkGlassRows();
  void proceedFallingRows();
  void createDropTrail(int x, int y, int height, Globals::Color color);
  void createRowFlash(int y);
  void deleteObsoleteEffects();

public:
  enum GameState { gsStartGame, gsPlayingGame, gsGameOver };
  GameState gameState;
  std::vector<Cell> glass;
  Figure nextFigures[Globals::nextFiguresCount];
  Figure holdFigure;
  Figure curFigure;
  int glassWidth = 10;
  int glassHeight = 20;
  int curFigureX;
  int curFigureY;
  int curLevel;
  bool forceDown;
  bool haveHold;
  bool justHolded;
  bool glassChanged;
  bool nextFiguresChanged;
  double startFallTimer;
  std::vector<int> rowElevation;
  std::list<DropTrail> dropTrails;
  std::list<RowFlash> rowFlashes;

  Model();
  ~Model();

  void update();

  float getRowCurrentElevation(int row);
  void holdCurrentFigure();
  void dropCurrentFigure();
  void rotateCurrentFigureLeft();
  void rotateCurrentFigureRight();
  void shiftCurrentFigureLeft();
  void shiftCurrentFigureRight();
  void storeCurFigureIntoGlass();
};
