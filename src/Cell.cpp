#include "static_headers.h"

#include "Cell.h"

Cell::Cell() :
  figureId(0),
  color(Color::clNone)
{

}

Cell::Cell(int figureId, Color color) :
  figureId(figureId),
  color(color)
{

}

void Cell::clear()
{
  figureId = 0;
  color = Color::clNone;
}

Cell & Cell::operator = (const Cell & cell)
{
  figureId = cell.figureId;
  color = cell.color;

  return *this;
}
