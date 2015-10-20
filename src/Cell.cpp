#include "Cell.h"

Cell::Cell() :
  figureId(0),
  fallHeight(0.0f),
  color(Globals::Color::clNone)
{

}

Cell::Cell(int figureId, Globals::Color color) :
  figureId(figureId),
  fallHeight(0.0f),
  color(color)
{

}

void Cell::clear()
{
  figureId = 0;
  fallHeight = 0.0f;
  color = Globals::Color::clNone;
}

Cell & Cell::operator = (const Cell & cell)
{
  figureId = cell.figureId;
  fallHeight = cell.fallHeight;
  color = cell.color;

  return *this;
}
