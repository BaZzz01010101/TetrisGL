#pragma once

#include "Cell.h"
#include "Figure.h"
#include "DropSparkle.h"
#include "DropTrail.h"
#include "Binding.h"
#include "MenuLogic.h"

#pragma warning(disable : 4512)

class GameLogic
{
public:
  enum State { stInit, stCountdown, stPlaying, stPaused, stGameOver, stStopped };
  enum Result { resNone, resGameOver };
  static State state;
  static std::vector<Cell> glass;
  static std::vector<Figure> nextFigures;
  static Figure holdFigure;
  static Figure curFigure;
  static int glassWidth;
  static int glassHeight;
  static int curFigureX;
  static int curFigureY;
  static int curScore;
  static int curGoal;
  static int curLevel;
  static bool haveHold;
  static bool haveFallingRows;
  static double rowsDeleteTimer;
  static bool menuButtonHighlighted;
  static unsigned int fastDownCounter;
  static const int countdownTime = 3;
  static float countdownTimeLeft;
  static float gameOverTimeLeft;

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
  static int getDropTrailsCount() { return (int)dropTrails.size(); }
  static DeletedRowsIterator getDeletedRowsBegin() { return deletedRows.begin(); }
  static DeletedRowsIterator getDeletedRowsEnd() { return deletedRows.end(); }
  static int getDeletedRowsCount() { return (int)deletedRows.size(); }
  static DeletedRowGapsIterator getDeletedRowGapsBegin() { return deletedRowGaps.begin(); }
  static DeletedRowGapsIterator getDeletedRowGapsEnd() { return deletedRowGaps.end(); }

  static void init();
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

  static void resetGame();
  static void gameUpdate();
  static float getStepTime();
  static void shiftFigureConveyor();
  static bool checkCurrentFigurePos(int dx, int dy);
  static bool tryToPlaceCurrentFigure();
  static void checkGlassRows();
  static void proceedFallingRows();
  static void createDropTrail(int x, int y, int height, Cell::Color color);
  static void deleteObsoleteEffects();
};
