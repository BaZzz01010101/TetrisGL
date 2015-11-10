#pragma once

#include "Cell.h"
#include "Figure.h"
#include "DropSparkle.h"
#include "DropTrail.h"
#include "Bindings.h"
#include "MenuLogic.h"

#pragma warning(disable : 4512)

class GameLogic
{
public:
  enum State { stInit, stPlaying, stPaused, stGameOver, stStopped };
  enum Result { resNone, resGameOver };
  ReadOnly<State, GameLogic> state;
  std::vector<Cell> glass;
  std::vector<Figure> nextFigures;
  Figure holdFigure;
  Figure curFigure;
  ReadOnly<int, GameLogic> glassWidth;
  ReadOnly<int, GameLogic> glassHeight;
  ReadOnly<int, GameLogic> curFigureX;
  ReadOnly<int, GameLogic> curFigureY;
  ReadOnly<int, GameLogic> curScore;
  ReadOnly<int, GameLogic> curGoal;
  ReadOnly<int, GameLogic> curLevel;
  ReadOnly<bool, GameLogic> haveHold;
  ReadOnly<bool, GameLogic> haveFallingRows;
  ReadOnly<double, GameLogic> rowsDeleteTimer;

  struct CellCoord
  {
    int x, y;
    inline CellCoord(int x, int y) : x(x), y(y) {}
    inline bool operator < (const CellCoord & left) const { return left.x < x || (left.x == x && left.y < y); }
  };

  typedef std::vector<int>::const_iterator DeletedRowsIterator;
  typedef std::list<DropTrail>::const_iterator DropTrailsIterator;
  typedef std::set<CellCoord>::const_iterator DeletedRowGapsIterator;

  GameLogic();
  ~GameLogic();

  Result update();

  void holdCurrentFigure();
  void fastDownCurrentFigure();
  void dropCurrentFigure();
  void rotateCurrentFigureLeft();
  void rotateCurrentFigureRight();
  void shiftCurrentFigureLeft();
  void shiftCurrentFigureRight();
  void storeCurFigureIntoGlass();
  void newGame() { state = stInit; }
  void pauseGame() { state = stPaused; }
  void continueGame() { state = stPlaying; }
  void stopGame() { state = stStopped; }

  DropTrailsIterator getDropTrailsBegin() const { return dropTrails.begin(); }
  DropTrailsIterator getDropTrailsEnd() const { return dropTrails.end(); }
  DeletedRowsIterator getDeletedRowsBegin() const { return deletedRows.begin(); }
  DeletedRowsIterator getDeletedRowsEnd() const { return deletedRows.end(); }
  DeletedRowGapsIterator getDeletedRowGapsBegin() const { return deletedRowGaps.begin(); }
  DeletedRowGapsIterator getDeletedRowGapsEnd() const { return deletedRowGaps.end(); }

  int getRowElevation(int y) const;
  float getRowCurrentElevation(int y) const;
  const Cell * getGlassCell(int x, int y) const;
  const Cell * getFigureCell(Figure & figure, int x, int y) const;

private:
  const int maxLevel;
  double lastStepTimer;
  bool justHolded;
  std::vector<int> rowElevation;
  std::vector<float> rowCurrentElevation;
  std::list<DropTrail> dropTrails;
  std::vector<int> deletedRows;
  std::set<CellCoord> deletedRowGaps;

  void initGame();
  void gameUpdate();
  float getStepTime() const;
  void shiftFigureConveyor();
  bool checkCurrentFigurePos(int dx, int dy);
  bool tryToPlaceCurrentFigure();
  void checkGlassRows();
  void proceedFallingRows();
  void createDropTrail(int x, int y, int height, Globals::Color color);
  void deleteObsoleteEffects();
};
