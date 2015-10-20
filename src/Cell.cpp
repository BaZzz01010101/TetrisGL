#include "Cell.h"

Cell::Cell() :
  figureId(0),
  color(Globals::Color::clNone)
{

}

Cell::Cell(int figureId, Globals::Color color) :
  figureId(figureId),
  color(color)
{

}
