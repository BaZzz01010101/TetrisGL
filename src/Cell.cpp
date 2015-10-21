#include "Cell.h"

Cell::Cell() :
  figureId(0),
  elevation(0),
  color(Globals::Color::clNone)
{

}

Cell::Cell(int figureId, Globals::Color color) :
  figureId(figureId),
  elevation(0),
  color(color)
{

}

void Cell::clear()
{
  figureId = 0;
  elevation = 0;
  color = Globals::Color::clNone;
}

Cell & Cell::operator = (const Cell & cell)
{
  figureId = cell.figureId;
  elevation = cell.elevation;
  color = cell.color;

  return *this;
}
