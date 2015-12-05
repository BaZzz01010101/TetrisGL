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
  static ReadOnly<State, GameLogic> state;
  static std::vector<Cell> glass;
  static std::vector<Figure> nextFigures;
  static Figure holdFigure;
  static Figure curFigure;
  static ReadOnly<int, GameLogic> glassWidth;
  static ReadOnly<int, GameLogic> glassHeight;
  static ReadOnly<int, GameLogic> curFigureX;
  static ReadOnly<int, GameLogic> curFigureY;
  static ReadOnly<int, GameLogic> curScore;
  static ReadOnly<int, GameLogic> curGoal;
  static ReadOnly<int, GameLogic> curLevel;
  static ReadOnly<bool, GameLogic> haveHold;
  static ReadOnly<bool, GameLogic> haveFallingRows;
  static ReadOnly<double, GameLogic> rowsDeleteTimer;

  struct CellCoord
  {
    int x, y;
    inline CellCoord(int x, int y) : x(x), y(y) {}
    inline bool operator < (const CellCoord & left) const { return left.x < x || (left.x == x && left.y < y); }
  };

  typedef std::vector<int>::const_iterator DeletedRowsIterator;
  typedef std::list<DropTrail>::const_iterator DropTrailsIterator;
  typedef std::set<CellCoord>::const_iterator DeletedRowGapsIterator;

  static Result update();

  static void holdCurrentFigure();
  static void fastDownCurrentFigure();
  static void dropCurrentFigure();
  static void rotateCurrentFigureLeft();
  static void rotateCurrentFigureRight();
  static void shiftCurrentFigureLeft();
  static void shiftCurrentFigureRight();
  static void storeCurFigureIntoGlass();
  static void newGame() { state = stInit; }
  static void pauseGame() { state = stPaused; }
  static void continueGame() { state = stPlaying; }
  static void stopGame() { state = stStopped; }

  static DropTrailsIterator getDropTrailsBegin() { return dropTrails.begin(); }
  static DropTrailsIterator getDropTrailsEnd() { return dropTrails.end(); }
  static DeletedRowsIterator getDeletedRowsBegin() { return deletedRows.begin(); }
  static DeletedRowsIterator getDeletedRowsEnd() { return deletedRows.end(); }
  static DeletedRowGapsIterator getDeletedRowGapsBegin() { return deletedRowGaps.begin(); }
  static DeletedRowGapsIterator getDeletedRowGapsEnd() { return deletedRowGaps.end(); }

  static int getRowElevation(int y);
  static float getRowCurrentElevation(int y);
  static const Cell * getGlassCell(int x, int y);
  static const Cell * getFigureCell(Figure & figure, int x, int y);

private:
  static const int maxLevel;
  static double lastStepTimer;
  static bool justHolded;
  static std::vector<int> rowElevation;
  static std::vector<float> rowCurrentElevation;
  static std::list<DropTrail> dropTrails;
  static std::vector<int> deletedRows;
  static std::set<CellCoord> deletedRowGaps;

  GameLogic();
  ~GameLogic();

  static void initGame();
  static void gameUpdate();
  static float getStepTime();
  static void shiftFigureConveyor();
  static bool checkCurrentFigurePos(int dx, int dy);
  static bool tryToPlaceCurrentFigure();
  static void checkGlassRows();
  static void proceedFallingRows();
  static void createDropTrail(int x, int y, int height, Globals::Color color);
  static void deleteObsoleteEffects();
};
