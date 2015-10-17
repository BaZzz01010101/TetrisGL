#include <assert.h>

#include "Model.h"
#include "Crosy.h"


Model::Model() :
  nextFiguresCount(3),
  maxLevel(100),
  maxStepTime(1.0f),
  minStepTime(0.05f),
  gameState(gsStartGame),
  curFigure(NULL),
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
  glass.assign(glassWidth * glassHeight, Cell(0, Cell::Color::clNone));
  nextFigures.clear();
  lastStepTime = getTime();
  
  for (int i = 0; i < nextFiguresCount; i++)
  {
    nextFigures.emplace_back();
    nextFigures.back().buildRandomFigure();
  }

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
  const float stepTime = maxStepTime - (maxStepTime - minStepTime) * curLevel / maxLevel;

  if (curTime > lastStepTime + stepTime)
  {
    if (checkCurrentFigurePos(0, 1))
    {
      curFigureY++;
      glassChanged = true;
    }
    else
      shiftFigureConveyor();

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

void Model::shiftFigureConveyor()
{
  if (curFigure)
  {
    int dim = curFigure->dim;

    for (int x = 0; x < dim; x++)
    for (int y = 0; y < dim; y++)
    if (curFigure->cells[x + y * dim])
      glass[curFigureX + x + (curFigureY + y) * glassWidth] = curFigure->cells[x + y * dim];
  }

  nextFigures.erase(nextFigures.begin());
  nextFigures.emplace_back();
  nextFigures.back().buildRandomFigure();
  curFigure = &nextFigures.front();
  curFigureX = (glassWidth - curFigure->dim) / 2;
  curFigureY = curFigure->dim / 2;

  if (!checkCurrentFigurePos(0, 0))
  {
    curFigure = NULL;
    gameState = gsGameOver;
  }

  glassChanged = true;
  nextFiguresChanged = true;
}

bool Model::checkCurrentFigurePos(int dx, int dy)
{
  assert(curFigure);

  for (int curx = 0; curx < curFigure->dim; curx++)
  for (int cury = 0; cury < curFigure->dim; cury++)
  {
    if (curFigure->cells[curx + cury * curFigure->dim])
    {
      if (curFigureX + curx + dx < 0 ||
      curFigureX + curx + dx >= glassWidth ||
      curFigureY + cury + dy < 0 ||
      curFigureY + cury + dy >= glassHeight ||
      glass[curFigureX + curx + dx + (curFigureY + cury + dy) * glassWidth]) 
        return false;
    }
  }

  return true;
}

bool Model::tryToRelocateCurrentFigure()
{
  assert(curFigure);

  if (checkCurrentFigurePos(0, 0))
    return true;

  const int shift = curFigure->dim / 2 + (curFigure->dim & 1);

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

void Model::dropCurrentFigure()
{
  assert(curFigure);

  int y0 = curFigureY;

  while (checkCurrentFigurePos(0, 1))
    curFigureY++;

  int y1 = curFigureY;
  int dim = curFigure->dim;

  if (y1 - y0 > 0)
  {
    for (int x = 0; x < dim; x++)
    for (int y = 0; y < dim; y++)
    {
      if (curFigure->cells[x + y * dim])
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
  shiftFigureConveyor();
  glassChanged = true;
}

void Model::rotateCurrentFigureLeft()
{
  Figure savedFigure = *curFigure;
  curFigure->rotate(Figure::rotLeft);

  if (!checkCurrentFigurePos(0, 0) && !tryToRelocateCurrentFigure())
    *curFigure = savedFigure;
  else
    glassChanged = true;
}

void Model::rotateCurrentFigureRight()
{
  Figure savedFigure = *curFigure;
  curFigure->rotate(Figure::rotRight);

  if (!checkCurrentFigurePos(0, 0) && !tryToRelocateCurrentFigure())
    *curFigure = savedFigure;
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
