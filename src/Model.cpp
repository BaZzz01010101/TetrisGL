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
  nextFiguresChanged(false),
  startFallTimer(-1.0)
{
}


Model::~Model()
{
}

void Model::initGameProceed(int glassWidth, int glassHeight)
{
  curLevel = 1;
  this->glassWidth = glassWidth;
  this->glassHeight = glassHeight;
  rowElevation.resize(glassHeight);
  glass.assign(glassWidth * glassHeight, Cell(0, Globals::Color::clNone));
  lastStepTimer = getTimer();
  
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
    initGameProceed(10, 20);
    gameState = gsPlayingGame;
    break;
  case gsPlayingGame:
    playingGameProceed();
    break;
  case gsGameOver:
    break;
  default:
    assert(0);
  }
}

void Model::playingGameProceed()
{
  double curTime = getTimer();
  const float forceDownStepTime = 0.025f;
  const float stepTime = forceDown ? forceDownStepTime : getStepTime();

  if (startFallTimer > 0.0)
    lastStepTimer = getTimer();
  else if (curTime > lastStepTimer + stepTime)
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

    lastStepTimer = curTime;
  }

  if (startFallTimer > 0.0)
    proceedFallingRows();

  deleteObsoleteEffects();
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

bool Model::tryToPlaceCurrentFigure()
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
    Figure savedCurFigure = curFigure;
    int saveCurFigureX = curFigureX;
    int saveCurFigureY = curFigureY;

    if (haveHold)
    {
      Figure::Type type = curFigure.type;
      curFigure = holdFigure;
      curFigureX = (glassWidth - curFigure.dim) / 2;
      curFigureY = curFigure.dim / 2;

      if (tryToPlaceCurrentFigure())
      {
        holdFigure.buildFigure(type);
        lastStepTimer = getTimer();
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
      curFigureY = curFigure.dim / 2;

      if (tryToPlaceCurrentFigure())
      {
        shiftFigureConveyor();
        haveHold = true;
        lastStepTimer = getTimer();
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
        createDropTrail(curFigureX + x, curFigureY + y, y1 - y0, curFigure.color);
        break;
      }
    }
  }

  lastStepTimer = getTimer();
  storeCurFigureIntoGlass();
  shiftFigureConveyor();
  glassChanged = true;
}

void Model::rotateCurrentFigureLeft()
{
  Figure savedFigure = curFigure;
  curFigure.rotate(Figure::rotLeft);

  if (!checkCurrentFigurePos(0, 0) && !tryToPlaceCurrentFigure())
    curFigure = savedFigure;
  else
    glassChanged = true;
}

void Model::rotateCurrentFigureRight()
{
  Figure savedFigure = curFigure;
  curFigure.rotate(Figure::rotRight);

  if (!checkCurrentFigurePos(0, 0) && !tryToPlaceCurrentFigure())
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
        createRowFlash(y);
      }
    else if (elevation)
    {
      for (int x = 0; x < glassWidth; x++)
      {
        glass[x + (y + elevation) * glassWidth] = glass[x + y * glassWidth];
        rowElevation[y + elevation] = elevation;
      }
    }
  }

  for (int i = 0; i < elevation * glassWidth; i++)
    glass[i].clear();

  if (elevation)
    startFallTimer = getTimer();
}

void Model::proceedFallingRows()
{
  bool stillFalling = false;

  if (startFallTimer > 0.0)
  {
    for (int y = 0; y < glassHeight; y++)
    for (int x = 0; x < glassWidth; x++)
    if (rowElevation[y])
    {
      if (getRowCurrentElevation(y) < 0.0f)
        rowElevation[y] = 0;
      else
        stillFalling = true;
    }

    if (!stillFalling)
      startFallTimer = -1.0f;
  }
}

float Model::getRowCurrentElevation(int row)
{
  if (startFallTimer < 0.0f)
    return 0.0f;

  const float acceleration = 50.0f;
  float timePassed = float(getTimer() - startFallTimer);

  return float(rowElevation[row]) - 0.5f * acceleration * timePassed * timePassed * timePassed;
}

void Model::createDropTrail(int x, int y, int height, Globals::Color color)
{
  dropTrails.emplace_back();
  DropTrail & dropTrail = dropTrails.back();
  dropTrail.x = x;
  dropTrail.y = y;
  dropTrail.color = color;
  dropTrail.height = height;
}

void Model::createRowFlash(int y)
{
  rowFlashes.emplace_back();
  RowFlash & rowFlash = rowFlashes.back();
  rowFlash.y = y;
}

void Model::deleteObsoleteEffects()
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

  std::list<RowFlash>::iterator rowFlash = rowFlashes.begin();

  while (rowFlash != rowFlashes.end())
  {
    if (rowFlash->getProgress() > 0.999f)
    {
      std::list<RowFlash>::iterator delIt = rowFlash++;
      rowFlashes.erase(delIt);
    }
    else ++rowFlash;
  }
}
