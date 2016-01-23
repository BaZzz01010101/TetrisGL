#include "static_headers.h"

#include "GameLogic.h"
#include "Crosy.h"
#include "Time.h"

GameLogic::State GameLogic::state = stStopped;
int GameLogic::curFigureX = 0;
int GameLogic::curFigureY = 0;
int GameLogic::curScore = 0;
int GameLogic::curGoal = 0;
int GameLogic::curLevel = 0;
bool GameLogic::haveHold = false;
bool GameLogic::haveFallingRows = false;
double GameLogic::rowsDeleteTimer = -1.0;
bool GameLogic::menuButtonHighlighted = false;
unsigned int GameLogic::fastDownCounter = 0;
unsigned int GameLogic::dropTrailCounter = 0;
float GameLogic::countdownTimeLeft = 0.0f;
float GameLogic::gameOverTimeLeft = 0.0f;
float GameLogic::rowsDeletionEffectTime = 0.8f;

Figure GameLogic::holdFigure;
Figure GameLogic::curFigure;
const int GameLogic::maxLevel = 20;
double GameLogic::lastStepTimer = -1.0f;
bool  GameLogic::justHolded = false;
std::vector<Cell> GameLogic::field;
std::vector<Figure> GameLogic::nextFigures;
std::vector<int> GameLogic::rowElevation;
std::vector<float> GameLogic::rowCurrentElevation;
std::vector<int> GameLogic::deletedRows;
std::vector<GameLogic::CellCoord> GameLogic::deletedRowGaps;
DropTrail GameLogic::dropTrails[dropTrailsSize];
int GameLogic::dropTrailsHead = 0;
int GameLogic::dropTrailsTail = 0;

void GameLogic::init()
{
  field.reserve(fieldWidth * fieldHeight);
  nextFigures.reserve(nextFiguresCount);
  rowElevation.reserve(fieldHeight);
  rowCurrentElevation.reserve(fieldHeight);
  deletedRows.reserve(fieldHeight);
  deletedRowGaps.reserve((fieldWidth + 1) * Figure::dimMax);
  resetGame();
}


void GameLogic::resetGame()
{
  curLevel = 1;
  curScore = 0;
  curGoal = 5;
  haveHold = false;
  holdFigure.clear();
  rowElevation.assign(fieldHeight, 0);
  rowCurrentElevation.assign(fieldHeight, 0.0f);
  nextFigures.resize(GameLogic::nextFiguresCount);
  field.assign(fieldWidth * fieldHeight, Cell(0, Cell::Color::clNone));
  lastStepTimer = Time::timer;
  
  for (int i = 0; i < GameLogic::nextFiguresCount; i++)
    nextFigures[i].buildRandomFigure();

  shiftFigureConveyor();
}


GameLogic::Result GameLogic::update()
{
  Result result = resNone;

  switch (state)
  {
    case stInit:
      resetGame();
      state = stCountdown;
      countdownTimeLeft = countdownTime + 0.99f;
      break;

    case stCountdown:
      countdownTimeLeft -= Time::timerDelta;

      if (countdownTimeLeft < 0.0f)
        state = stPlaying;

      break;

    case stPlaying:
      gameUpdate();
      break;
    case stPaused:
      break;

    case stGameOver:
      gameOverTimeLeft -= Time::timerDelta;

      if (gameOverTimeLeft < 0.0f)
      {
        state = stStopped;
        result = resGameOver;
      }

      break;

    case stStopped:
      break;
    default:
      assert(0);
      break;
  }

  return result;
}


void GameLogic::gameUpdate()
{
  const float stepTime = getStepTime();

  if (haveFallingRows)
    lastStepTimer = Time::timer;
  else if (Time::timer > lastStepTimer + stepTime)
  {
    // TODO : fix posible wrong behavior on extremely low FPS
    //        (falling speed will be limited by fps)
    if (checkCurrentFigurePos(0, 1))
      curFigureY++;
    else
    {
      storeCurFigureIntoField();
      checkFieldRows();
      shiftFigureConveyor();
    }

    lastStepTimer = Time::timer;
  }

  proceedFallingRows();
  updateEffects();
}


float GameLogic::getStepTime()
{
  const float maxStepTime = 1.0f;
  const float minStepTime = 0.0f;
  float opRelLevel = 1.0f - float(curLevel) / maxLevel;
  float k = 1.0f - opRelLevel * opRelLevel;

  return maxStepTime - (maxStepTime - minStepTime) * k;
}


void GameLogic::storeCurFigureIntoField()
{
  int dim = curFigure.dim;

  for (int x = 0; x < dim; x++)
    for (int y = 0; y < dim; y++)
      if (!curFigure.cells[x + y * dim].isEmpty())
        field[curFigureX + x + (curFigureY + y) * fieldWidth] = curFigure.cells[x + y * dim];
}


void GameLogic::shiftFigureConveyor()
{
  justHolded = false;

  Figure::swap(curFigure, nextFigures[0]);

  for (int i = 1; i < GameLogic::nextFiguresCount; i++)
    Figure::swap(nextFigures[i - 1], nextFigures[i]);

  nextFigures[GameLogic::nextFiguresCount - 1].buildRandomFigure();

  curFigureX = (fieldWidth - curFigure.dim) / 2;
  // TODO : fix L - figure appearance vertical coordinate
  curFigureY = 0;

  if (!checkCurrentFigurePos(0, 0))
  {
    curFigure.clear();
    state = stGameOver;
    gameOverTimeLeft = gameOverTime;
  }
}


bool GameLogic::checkCurrentFigurePos(int dx, int dy)
{
  for (int curx = 0; curx < curFigure.dim; curx++)
    for (int cury = 0; cury < curFigure.dim; cury++)
      if (!curFigure.cells[curx + cury * curFigure.dim].isEmpty() && 
          curFigureY + cury + dy >= 0)
      {
        if (curFigureX + curx + dx < 0 ||
            curFigureX + curx + dx >= fieldWidth ||
            curFigureY + cury + dy >= fieldHeight ||
            !field[curFigureX + curx + dx + (curFigureY + cury + dy) * fieldWidth].isEmpty())
        {
          return false;
        }
      }

  return true;
}


bool GameLogic::tryToPlaceCurrentFigure()
{
  if (checkCurrentFigurePos(0, 0))
    return true;

  const int shift = curFigure.dim / 2 + (curFigure.dim & 1);

  for (int dx = 1; dx <= shift; dx++)
  {
    if (checkCurrentFigurePos(dx, 0))
    {
      curFigureX += dx;
      return true;
    }

    if (checkCurrentFigurePos(-dx, 0))
    {
      curFigureX -= dx;
      return true;
    }
  }

  return false;
}


void GameLogic::holdCurrentFigure()
{
  if (!justHolded)
  {
    Figure savedCurFigure = curFigure;
    int saveCurFigureX = curFigureX;
    int saveCurFigureY = curFigureY;

    if (haveHold)
    {
      Figure::swap(holdFigure, curFigure);
      curFigureX = (fieldWidth - curFigure.dim) / 2;
      curFigureY = 0;

      if (tryToPlaceCurrentFigure())
      {
        lastStepTimer = Time::timer;
        justHolded = true;
      }
      else
      {
        Figure::swap(holdFigure, curFigure);
        curFigureX = saveCurFigureX;
        curFigureY = saveCurFigureY;
      }
    }
    else
    {
      holdFigure = curFigure;
      curFigure = nextFigures[0];
      curFigureX = (fieldWidth - curFigure.dim) / 2;
      curFigureY = 0;

      if (tryToPlaceCurrentFigure())
      {
        shiftFigureConveyor();
        haveHold = true;
        lastStepTimer = Time::timer;
        justHolded = true;
      }
      else
      {
        holdFigure.clear();
        curFigure = savedCurFigure;
        curFigureX = saveCurFigureX;
        curFigureY = saveCurFigureY;
      }
    }
  }
}


bool GameLogic::fastDownCurrentFigure()
{
  bool result = false;

  if (checkCurrentFigurePos(0, 1))
    curFigureY++;
  else
  {
    storeCurFigureIntoField();
    checkFieldRows();
    shiftFigureConveyor();
    result = true;
  }

  lastStepTimer = Time::timer;
  fastDownCounter++;

  return result;
}


void GameLogic::dropCurrentFigure()
{
  int y0 = curFigureY;

  while (checkCurrentFigurePos(0, 1))
    curFigureY++;

  int y1 = curFigureY;
  int dim = curFigure.dim;

  if (y1 - y0 > 0)
  {
    curScore += (y1 - y0) / 2;

    for (int x = 0; x < dim; x++)
      for (int y = 0; y < dim; y++)
        if (!curFigure.cells[x + y * dim].isEmpty())
        {
          addDropTrail(curFigureX + x, curFigureY + y, y1 - y0, curFigure.color);
          break;
        }
  }

  lastStepTimer = Time::timer;
  storeCurFigureIntoField();
  checkFieldRows();
  shiftFigureConveyor();
}


void GameLogic::rotateCurrentFigureLeft()
{
  Figure savedFigure = curFigure;
  curFigure.rotateLeft();

  if (!checkCurrentFigurePos(0, 0) && !tryToPlaceCurrentFigure())
    curFigure = savedFigure;
}


void GameLogic::rotateCurrentFigureRight()
{
  Figure savedFigure = curFigure;
  curFigure.rotateRight();

  if (!checkCurrentFigurePos(0, 0) && !tryToPlaceCurrentFigure())
    curFigure = savedFigure;
}


void GameLogic::shiftCurrentFigureLeft()
{
  if (checkCurrentFigurePos(-1, 0))
    curFigureX--;
}


void GameLogic::shiftCurrentFigureRight()
{
  if (checkCurrentFigurePos(1, 0))
    curFigureX++;
}


void GameLogic::checkFieldRows()
{
  int elevation = 0;

  for (int y = fieldHeight - 1; y >= 0; y--)
  {
    bool fullRow = true;

    for (int x = 0; x < fieldWidth; x++)
      if (field[x + y * fieldWidth].isEmpty())
        fullRow = false;

    if (fullRow)
    {
      elevation++;
      deletedRows.push_back(y);
      addRowGaps(y);
    }
    else if (elevation)
    {
      for (int x = 0; x < fieldWidth; x++)
      {
        field[x + (y + elevation) * fieldWidth] = field[x + y * fieldWidth];
        rowElevation[y + elevation] = elevation;
        rowCurrentElevation[y + elevation] = (float)elevation;
      }
    }
  }

  for (int i = 0; i < elevation * fieldWidth; i++)
    field[i].clear();

  if (elevation)
  {
    haveFallingRows = true;
    rowsDeleteTimer = Time::timer;
    curScore += elevation * elevation * 10;
    curGoal -= (int)pow(elevation, 1.5f);

    if (curGoal <= 0)
    {
      curLevel++;
      curGoal = curLevel * 5;
    }
  }
}


void GameLogic::proceedFallingRows()
{
  if (haveFallingRows)
  {
    haveFallingRows = false;
    float timePassed = float(Time::timer - rowsDeleteTimer);

    for (int y = 0; y < fieldHeight; y++)
      for (int x = 0; x < fieldWidth; x++)
        if (rowElevation[y] > 0)
        {
          rowCurrentElevation[y] = glm::max(float(rowElevation[y]) - 
                                            25.0f * timePassed * timePassed * timePassed, 0.0f);

          if (rowCurrentElevation[y] > 0.0f)
            haveFallingRows = true;
          else
            rowElevation[y] = 0;
        }
  }
}


void GameLogic::addDropTrail(int x, int y, int height, Cell::Color color)
{
  int newHead = (dropTrailsHead + 1) % dropTrailsSize;

  if (newHead != dropTrailsTail)
  {
    DropTrail & dropTrail = dropTrails[dropTrailsHead];
    dropTrail.set(x, y, height, color);
    dropTrailCounter++;
    dropTrailsHead = newHead;
  }
}


void GameLogic::addRowGaps(int y)
{
  // TODO : move first and last gap addition out of cycle
  for (int x = 0; x <= fieldWidth; x++)
    if (!x || x == fieldWidth ||
        field[x + y * fieldWidth].figureId != field[x - 1 + y * fieldWidth].figureId)
    {
      deletedRowGaps.push_back(CellCoord(x, y));
    }
}


void GameLogic::updateEffects()
{
  for (int i = dropTrailsTail; i != dropTrailsHead; i = (i + 1) % dropTrailsSize)
    if (!dropTrails[i].update(Time::timerDelta))
      dropTrailsTail = (i + 1) % dropTrailsSize;

  if (!deletedRows.empty() && Time::timer - rowsDeleteTimer > GameLogic::rowsDeletionEffectTime)
  {
    deletedRows.clear();
    deletedRowGaps.clear();
  }
}


int GameLogic::getRowElevation(int y)
{
  assert(y >= 0);
  assert(y < fieldHeight);

  return (y >= 0 && y < fieldHeight) ? rowElevation[y] : 0;
}


float GameLogic::getRowCurrentElevation(int y) 
{
  assert(y >= 0);
  assert(y < fieldHeight);

  return (y >= 0 && y < fieldHeight) ? rowCurrentElevation[y] : 0.0f;
}


// cell of the game field considering current figure
// returns NULL if out of field bounds
const Cell * GameLogic::getFieldCell(int x, int y) 
{
  if (x < 0 || y < 0 || x >= fieldWidth || y >= fieldHeight)
    return NULL;

  const Cell * cell = &field[x + y * fieldWidth];

  if (!cell->figureId && !haveFallingRows)
  {
    bool isInCurFigure =
      x >= curFigureX &&
      x < curFigureX + curFigure.dim &&
      y >= curFigureY &&
      y < curFigureY + curFigure.dim;

    if (isInCurFigure)
      cell = &curFigure.cells[x - curFigureX + (y - curFigureY) * curFigure.dim];
  }

  return cell;
}


// TODO : move function to the Figure class
const Cell * GameLogic::getFigureCell(Figure & figure, int x, int y)
{
  if (x < 0 || y < 0 || x >= figure.dim || y >= figure.dim)
    return NULL;

  Cell * cell = &figure.cells[x + y * figure.dim];

  return cell;
}

