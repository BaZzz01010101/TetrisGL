#include "static_headers.h"

#include "Model.h"
#include "Crosy.h"

Model::Model() :
  maxLevel(20),
  gameState(gsStartGame),
  forceDown(false),
  haveHold(false),
  justHolded(false),
  glassChanged(false),
  haveFallingRows(false),
  nextFiguresChanged(false),
  showWireframe(false),
  rowsDeleteTimer(-1.0),
  rowsDeletionEffectTime(0.8f)
{
}


Model::~Model()
{
}

void Model::initGameProceed(int glassWidth, int glassHeight)
{
  curLevel = 1;
  curScore = 0;
  curGoal = 5;
  this->glassWidth = glassWidth;
  this->glassHeight = glassHeight;
  rowElevation.assign(glassHeight, 0);
  rowCurrentElevation.assign(glassHeight, 0.0f);
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
  const float forceDownStepTime = 0.04f;
  const float stepTime = forceDown ? forceDownStepTime : getStepTime();

  if (haveFallingRows)
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

  proceedFallingRows();

  deleteObsoleteEffects();
}

float Model::getStepTime()
{
  const float maxStepTime = 1.0f;
  const float minStepTime = 0.0f;
  float opRelLevel = 1.0f - float(curLevel) / maxLevel;
  float k = 1.0f - opRelLevel * opRelLevel;

  return maxStepTime - (maxStepTime - minStepTime) * k;
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
  curFigureY = 0;// -curFigure.dim / 2;

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
    if (!curFigure.cells[curx + cury * curFigure.dim].isEmpty() && curFigureY + cury + dy >= 0)
    {
      if (curFigureX + curx + dx < 0 ||
      curFigureX + curx + dx >= glassWidth ||
      //curFigureY + cury + dy < 0 ||
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

  lastStepTimer = getTimer();
  storeCurFigureIntoGlass();
  shiftFigureConveyor();
  glassChanged = true;
}

void Model::rotateCurrentFigureLeft()
{
  Figure savedFigure = curFigure;
  curFigure.rotateLeft();

  if (!checkCurrentFigurePos(0, 0) && !tryToPlaceCurrentFigure())
    curFigure = savedFigure;
  else
    glassChanged = true;
}

void Model::rotateCurrentFigureRight()
{
  Figure savedFigure = curFigure;
  curFigure.rotateRight();

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
    rowsDeleteTimer = getTimer();
    curScore += elevation * elevation * 10;
    curGoal -= (int)pow(elevation, 1.5f);

    if (curGoal <= 0)
    {
      curLevel++;
      curGoal = curLevel * 5;
    }
//    curLevel = 0.5f * (sqrt(1 + 8 * curScore / 100) - 1) + 1;
//    curGoal = curLevel * 10;
  }
}

void Model::proceedFallingRows()
{
  if (haveFallingRows)
  {
    haveFallingRows = false;
    float timePassed = float(getTimer() - rowsDeleteTimer);

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

void Model::createDropTrail(int x, int y, int height, Globals::Color color)
{
  dropTrails.emplace_back();
  DropTrail & dropTrail = dropTrails.back();
  dropTrail.x = x;
  dropTrail.y = y;
  dropTrail.color = color;
  dropTrail.height = height;
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

  if (!deletedRows.empty() && getTimer() - rowsDeleteTimer > rowsDeletionEffectTime)
  {
    deletedRows.clear();
    deletedRowGaps.clear();
  }
}
