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
  static std::vector<Cell> field;
  static std::vector<Figure> nextFigures;
  static Figure holdFigure;
  static Figure curFigure;
  static const int fieldWidth = 10;
  static const int fieldHeight = 20;
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
  static unsigned int dropTrailCounter;
  static const int countdownTime = 3;
  static float countdownTimeLeft;
  static const int gameOverTime = 3;
  static float gameOverTimeLeft;
  static int nextFiguresCount;
  static float rowsDeletionEffectTime;
  static const int dropTrailsSize = fieldWidth * fieldHeight;
  static DropTrail dropTrails[dropTrailsSize];
  static int dropTrailsHead;
  static int dropTrailsTail;

  struct CellCoord
  {
    int x, y;
    inline CellCoord(int x, int y) : x(x), y(y) {}
    inline bool operator < (const CellCoord & left) const { return left.x < x || (left.x == x && left.y < y); }
  };

  typedef std::vector<int>::const_iterator DeletedRowsIterator;
  typedef std::vector<CellCoord>::const_iterator DeletedRowGapsIterator;

  static Result update();

  static void holdCurrentFigure();
  static bool fastDownCurrentFigure();
  static void dropCurrentFigure();
  static void rotateCurrentFigureLeft();
  static void rotateCurrentFigureRight();
  static void shiftCurrentFigureLeft();
  static void shiftCurrentFigureRight();
  static void storeCurFigureIntoField();
  static void newGame() { state = stInit; }
  static void pauseGame() { state = stPaused; }
  static void continueGame() { state = stPlaying; }
  static void stopGame() { state = stStopped; }

  static DeletedRowsIterator getDeletedRowsBegin() { return deletedRows.begin(); }
  static DeletedRowsIterator getDeletedRowsEnd() { return deletedRows.end(); }
  static int getDeletedRowsCount() { return (int)deletedRows.size(); }
  static DeletedRowGapsIterator getDeletedRowGapsBegin() { return deletedRowGaps.begin(); }
  static DeletedRowGapsIterator getDeletedRowGapsEnd() { return deletedRowGaps.end(); }

  static void init();
  static int getRowElevation(int y);
  static float getRowCurrentElevation(int y);
  static const Cell * getFieldCell(int x, int y);
  static const Cell * getFigureCell(Figure & figure, int x, int y);

private:
  static const int maxLevel;
  static double lastStepTimer;
  static bool justHolded;
  static std::vector<int> rowElevation;
  static std::vector<float> rowCurrentElevation;
  static std::vector<int> deletedRows; 
  static std::vector<CellCoord> deletedRowGaps;

  GameLogic();
  ~GameLogic();

  static void resetGame();
  static void gameUpdate();
  static float getStepTime();
  static void shiftFigureConveyor();
  static bool checkCurrentFigurePos(int dx, int dy);
  static bool tryToPlaceCurrentFigure();
  static void checkFieldRows();
  static void proceedFallingRows();
  static void addDropTrail(int x, int y, int height, Cell::Color color);
  static void addRowGaps(int y);
  static void updateEffects();
};
