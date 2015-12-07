#include "static_headers.h"

#include "GameLogic.h"
#include "Crosy.h"
#include "Time.h"

GameLogic::State GameLogic::state = stStopped;
int GameLogic::glassWidth = 10;
int GameLogic::glassHeight = 20;
int GameLogic::curFigureX = 0;
int GameLogic::curFigureY = 0;
int GameLogic::curScore = 0;
int GameLogic::curGoal = 0;
int GameLogic::curLevel = 0;
bool GameLogic::haveHold = false;
bool GameLogic::haveFallingRows = false;
double GameLogic::rowsDeleteTimer = -1.0;

Figure GameLogic::holdFigure;
Figure GameLogic::curFigure;
const int GameLogic::maxLevel = 20;
double GameLogic::lastStepTimer = -1.0f;
bool  GameLogic::justHolded = false;
std::vector<Cell> GameLogic::glass;
std::vector<Figure> GameLogic::nextFigures;
std::vector<int> GameLogic::rowElevation;
std::vector<float> GameLogic::rowCurrentElevation;
std::list<DropTrail> GameLogic::dropTrails;
std::vector<int> GameLogic::deletedRows;
std::set<GameLogic::CellCoord> GameLogic::deletedRowGaps;

void GameLogic::init()
{
  glassWidth = 10;
  glassHeight = 20;

  resetGame();
}

void GameLogic::resetGame()
{
  curLevel = 1;
  curScore = 0;
  curGoal = 5;
  haveHold = false;
  holdFigure.clear();
  rowElevation.assign(glassHeight, 0);
  rowCurrentElevation.assign(glassHeight, 0.0f);
  nextFigures.resize(Globals::nextFiguresCount);
  glass.assign(glassWidth * glassHeight, Cell(0, Globals::Color::clNone));
  lastStepTimer = Time::timer;
  
  for (int i = 0; i < Globals::nextFiguresCount; i++)
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
    state = stPlaying;
    break;

  case stPlaying:
    gameUpdate();
    break;

  case stPaused:
    break;

  case stGameOver:
    state = stStopped;
    result = resGameOver;
    break;

  case stStopped:
    break;

  default:
    assert(0);
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
    if (checkCurrentFigurePos(0, 1))
      curFigureY++;
    else
    {
      storeCurFigureIntoGlass();
      shiftFigureConveyor();
    }

    lastStepTimer = Time::timer;
  }

  proceedFallingRows();

  deleteObsoleteEffects();
}

float GameLogic::getStepTime()
{
  const float maxStepTime = 1.0f;
  const float minStepTime = 0.0f;
  float opRelLevel = 1.0f - float(curLevel) / maxLevel;
  float k = 1.0f - opRelLevel * opRelLevel;

  return maxStepTime - (maxStepTime - minStepTime) * k;
}

void GameLogic::storeCurFigureIntoGlass()
{
  int dim = curFigure.dim;

  for (int x = 0; x < dim; x++)
  for (int y = 0; y < dim; y++)
  if (!curFigure.cells[x + y * dim].isEmpty())
    glass[curFigureX + x + (curFigureY + y) * glassWidth] = curFigure.cells[x + y * dim];

  checkGlassRows();
}

void GameLogic::shiftFigureConveyor()
{
  justHolded = false;

  Figure::swap(curFigure, nextFigures[0]);

  for (int i = 1; i < Globals::nextFiguresCount; i++)
    Figure::swap(nextFigures[i - 1], nextFigures[i]);

  nextFigures[Globals::nextFiguresCount - 1].buildRandomFigure();

  curFigureX = (glassWidth - curFigure.dim) / 2;
  curFigureY = 0;

  if (!checkCurrentFigurePos(0, 0))
  {
    curFigure.clear();
    state = stGameOver;
  }
}

bool GameLogic::checkCurrentFigurePos(int dx, int dy)
{
  for (int curx = 0; curx < curFigure.dim; curx++)
  for (int cury = 0; cury < curFigure.dim; cury++)
  {
    if (!curFigure.cells[curx + cury * curFigure.dim].isEmpty() && curFigureY + cury + dy >= 0)
    {
      if (curFigureX + curx + dx < 0 ||
      curFigureX + curx + dx >= glassWidth ||
      curFigureY + cury + dy >= glassHeight ||
      !glass[curFigureX + curx + dx + (curFigureY + cury + dy) * glassWidth].isEmpty()) 
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
      Figure::Type type = curFigure.type;
      curFigure = holdFigure;
      curFigureX = (glassWidth - curFigure.dim) / 2;
      curFigureY = 0;

      if (tryToPlaceCurrentFigure())
      {
        holdFigure.buildFigure(type);
        lastStepTimer = Time::timer;
        justHolded = true;
      }
      else
      {
        curFigure = savedCurFigure;
        curFigureX = saveCurFigureX;
        curFigureY = saveCurFigureY;
      }
    }
    else
    {
      holdFigure = curFigure;
      curFigure = nextFigures[0];
      curFigureX = (glassWidth - curFigure.dim) / 2;
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

void GameLogic::fastDownCurrentFigure()
{
  if (checkCurrentFigurePos(0, 1))
    curFigureY++;
  else
  {
    storeCurFigureIntoGlass();
    shiftFigureConveyor();
  }

  lastStepTimer = Time::timer;
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
    {
      if (!curFigure.cells[x + y * dim].isEmpty())
      {
        createDropTrail(curFigureX + x, curFigureY + y, y1 - y0, curFigure.color);
        break;
      }
    }
  }

  lastStepTimer = Time::timer;
  storeCurFigureIntoGlass();
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

void GameLogic::checkGlassRows()
{
  int elevation = 0;

  for (int y = glassHeight - 1; y >= 0; y--)
  {
    bool fullRow = true;

    for (int x = 0; x < glassWidth; x++)
      if (glass[x + y * glassWidth].isEmpty())
        fullRow = false;

    if (fullRow)
    {
      elevation++;
      deletedRows.push_back(y);

      for (int x = 0; x <= glassWidth; x++)
      {
        if (!x || x == glassWidth || glass[x + y * glassWidth].figureId != glass[x - 1 + y * glassWidth].figureId)
          deletedRowGaps.insert(CellCoord(x, y));
      }
    }
    else if (elevation)
    {
      for (int x = 0; x < glassWidth; x++)
      {
        glass[x + (y + elevation) * glassWidth] = glass[x + y * glassWidth];
        rowElevation[y + elevation] = elevation;
        rowCurrentElevation[y + elevation] = (float)elevation;
      }
    }
  }

  for (int i = 0; i < elevation * glassWidth; i++)
    glass[i].clear();

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

    for (int y = 0; y < glassHeight; y++)
    for (int x = 0; x < glassWidth; x++)
    if (rowElevation[y] > 0)
    {
      rowCurrentElevation[y] = glm::max(float(rowElevation[y]) - 25.0f * timePassed * timePassed * timePassed, 0.0f);

      if (rowCurrentElevation[y] > 0.0f)
        haveFallingRows = true;
      else
        rowElevation[y] = 0;
    }
  }
}

void GameLogic::createDropTrail(int x, int y, int height, Globals::Color color)
{
  dropTrails.emplace_back();
  DropTrail & dropTrail = dropTrails.back();
  dropTrail.x = x;
  dropTrail.y = y;
  dropTrail.color = color;
  dropTrail.height = height;
}

void GameLogic::deleteObsoleteEffects()
{
  std::list<DropTrail>::iterator dropTrail = dropTrails.begin();

  while (dropTrail != dropTrails.end())
  {
    if (dropTrail->getTrailProgress() > 0.999f)
    {
      std::list<DropTrail>::iterator delIt = dropTrail++;
      dropTrails.erase(delIt);
    }
    else ++dropTrail;
  }

  if (!deletedRows.empty() && Time::timer - rowsDeleteTimer > Globals::rowsDeletionEffectTime)
  {
    deletedRows.clear();
    deletedRowGaps.clear();
  }
}

int GameLogic::getRowElevation(int y)
{
  assert(y >= 0);
  assert(y < glassHeight);

  return (y >= 0 && y < glassHeight) ? rowElevation[y] : 0;
}

float GameLogic::getRowCurrentElevation(int y) 
{
  assert(y >= 0);
  assert(y < glassHeight);

  return (y >= 0 && y < glassHeight) ? rowCurrentElevation[y] : 0.0f;
}

const Cell * GameLogic::getGlassCell(int x, int y) 
{
  if (x < 0 || y < 0 || x >= glassWidth || y >= glassHeight)
    return NULL;

  const Cell * cell = glass.data() + x + y * glassWidth;

  if (!cell->figureId && !haveFallingRows)
  {
    bool isInCurFigure =
      x >= curFigureX &&
      x < curFigureX + curFigure.dim &&
      y >= curFigureY &&
      y < curFigureY + curFigure.dim;

    if (isInCurFigure)
      cell = curFigure.cells.data() + x - curFigureX + (y - curFigureY) * curFigure.dim;
  }

  return cell;
}

const Cell * GameLogic::getFigureCell(Figure & figure, int x, int y) 
{
  if (x < 0 || y < 0 || x >= figure.dim || y >= figure.dim)
    return NULL;

  Cell * cell = figure.cells.data() + x + y * figure.dim;
  return cell;
}

