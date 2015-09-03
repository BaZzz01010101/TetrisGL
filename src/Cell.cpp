#include "Cell.h"

Cell::Cell() :
  figureId(0),
  color(clNone)
{

}

Cell::Cell(int figureId, Color color) : 
  figureId(figureId),
  color(color)
{

}
