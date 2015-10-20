#include <assert.h>

#include "Model.h"
#include "Crosy.h"

Model::Model() :
  maxLevel(100),
  maxStepTime(1.0f),
  minStepTime(0.05f),
  gameState(gsStartGame),
  forceDown(false),
  haveHold(false),
  justHolded(false),
  glassChanged(false),
  nextFiguresChanged(false)
{
}


Model::~Model()
{
}

void Model::initGame(int glassWidth, int glassHeight)
{
  curLevel = 1;
  this->glassWidth = glassWidth;
  this->glassHeight = glassHeight;
  glass.assign(glassWidth * glassHeight, Cell(0, Globals::Color::clNone));
  lastStepTime = getTime();
  
  for (int i = 0; i < Globals::nextFiguresCount; i++)
    nextFigures[i].buildRandomFigure();

  shiftFigureConveyor();
  glassChanged = true;
  nextFiguresChanged = true;
}

void Model::update()
{
  switch (gameState)
  {
  case gsStartGame:
    initGame(10, 20);
    gameState = gsPlayingGame;
    break;
  case gsPlayingGame:
    pulse();
    break;
  case gsGameOver:
    break;
  default:
    assert(0);
  }
}

void Model::pulse()
{
  float curTime = getTime();
  const float forceDownStepTime = 0.025f;
  const float stepTime = forceDown ? forceDownStepTime : getStepTime();

  if (curTime > lastStepTime + stepTime)
  {
    if (checkCurrentFigurePos(0, 1))
    {
      curFigureY++;
      glassChanged = true;
    }
    else
    {
      storeCurFigureIntoGlass();
      shiftFigureConveyor();
    }

    lastStepTime = curTime;
  }

//  cleanupDropTrails();
}

float Model::getTime()
{
  static uint64_t freq = Crosy::getPerformanceFrequency();
  assert(freq);

  return float(Crosy::getPerformanceCounter()) / freq;
}

float Model::getStepTime()
{
  return maxStepTime - (maxStepTime - minStepTime) * curLevel / maxLevel;
}

void Model::storeCurFigureIntoGlass()
{
  int dim = curFigure.dim;

  for (int x = 0; x < dim; x++)
  for (int y = 0; y < dim; y++)
  if (!curFigure.cells[x + y * dim].isEmpty())
    glass[curFigureX + x + (curFigureY + y) * glassWidth] = curFigure.cells[x + y * dim];

  checkGlassRows();
}

void Model::shiftFigureConveyor()
{
  forceDown = false;
  justHolded = false;

  Figure::swap(curFigure, nextFigures[0]);

  for (int i = 1; i < Globals::nextFiguresCount; i++)
    Figure::swap(nextFigures[i - 1], nextFigures[i]);

  nextFigures[Globals::nextFiguresCount - 1].buildRandomFigure();

  curFigureX = (glassWidth - curFigure.dim) / 2;
  curFigureY = curFigure.dim / 2;

  if (!checkCurrentFigurePos(0, 0))
  {
    curFigure.clear();
    gameState = gsGameOver;
  }

  glassChanged = true;
  nextFiguresChanged = true;
}

bool Model::checkCurrentFigurePos(int dx, int dy)
{
  for (int curx = 0; curx < curFigure.dim; curx++)
  for (int cury = 0; cury < curFigure.dim; cury++)
  {
    if (!curFigure.cells[curx + cury * curFigure.dim].isEmpty())
    {
      if (curFigureX + curx + dx < 0 ||
      curFigureX + curx + dx >= glassWidth ||
      curFigureY + cury + dy < 0 ||
      curFigureY + cury + dy >= glassHeight ||
      !glass[curFigureX + curx + dx + (curFigureY + cury + dy) * glassWidth].isEmpty()) 
        return false;
    }
  }

  return true;
}

bool Model::tryToRelocateCurrentFigure()
{
  if (checkCurrentFigurePos(0, 0))
    return true;

  const int shift = curFigure.dim / 2 + (curFigure.dim & 1);

  for (int dx = 1; dx < shift; dx++)
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

void Model::holdCurrentFigure()
{
  if (!justHolded)
  {
    if (haveHold)
    {
      Figure::Type type = curFigure.type;
      curFigure = holdFigure;
      holdFigure.buildFigure(type);
    }
    else
    {
      holdFigure = curFigure;
      shiftFigureConveyor();
      haveHold = true;
    }

    curFigureY = curFigure.dim / 2;
    lastStepTime = getTime();
    justHolded = true;
  }
}

void Model::dropCurrentFigure()
{
  int y0 = curFigureY;

  while (checkCurrentFigurePos(0, 1))
    curFigureY++;

  int y1 = curFigureY;
  int dim = curFigure.dim;

  if (y1 - y0 > 0)
  {
    for (int x = 0; x < dim; x++)
    for (int y = 0; y < dim; y++)
    {
      if (!curFigure.cells[x + y * dim].isEmpty())
      {
        //dropTrails.emplace_back();
        //DropTrail & dropTrail = dropTrails.back();
        //dropTrail.height = std::fmax(float(y1 - y0), 5.0f);
        //dropTrail.pos = { Globals::glassPos.x + (curFigureX + x) * glassSqSize, Globals::glassPos.y - (y1 + y - dropTrail.height) * glassSqSize };
        //dropTrail.color = curFigureXor;

        break;
      }
    }
  }

  lastStepTime = getTime();
  storeCurFigureIntoGlass();
  shiftFigureConveyor();
  glassChanged = true;
}

void Model::rotateCurrentFigureLeft()
{
  Figure savedFigure = curFigure;
  curFigure.rotate(Figure::rotLeft);

  if (!checkCurrentFigurePos(0, 0) && !tryToRelocateCurrentFigure())
    curFigure = savedFigure;
  else
    glassChanged = true;
}

void Model::rotateCurrentFigureRight()
{
  Figure savedFigure = curFigure;
  curFigure.rotate(Figure::rotRight);

  if (!checkCurrentFigurePos(0, 0) && !tryToRelocateCurrentFigure())
    curFigure = savedFigure;
  else
    glassChanged = true;
}

void Model::shiftCurrentFigureLeft()
{
  if (checkCurrentFigurePos(-1, 0))
  {
    curFigureX--;
    glassChanged = true;
  }
}

void Model::shiftCurrentFigureRight()
{
  if (checkCurrentFigurePos(1, 0))
  {
    curFigureX++;
    glassChanged = true;
  }
}

void Model::checkGlassRows()
{
  int fallHeight = 0;

  for (int y = glassHeight - 1; y >= 0; y--)
  {
    bool fullRow = true;

    for (int x = 0; x < glassWidth; x++)
      if (glass[x + y * glassWidth].isEmpty())
        fullRow = false;

    if (fullRow)
      fallHeight++;
    else if (fallHeight)
    {
      for (int x = 0; x < glassWidth; x++)
      {
        glass[x + y * glassWidth].fallHeight = (float)fallHeight;
        glass[x + (y + fallHeight) * glassWidth] = glass[x + y * glassWidth];
      }
    }
  }

  for (int i = 0; i < fallHeight * glassWidth; i++)
    glass[i].clear();
}
