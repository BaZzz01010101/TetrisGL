#include "static_headers.h"

#include "Figure.h"

int Figure::nextId = 1;

Figure::Figure() :
  haveSpecificRotation(false),
  specificRotatedFlag(false),
  type(typeNone),
  dim(0),
  color(Globals::Color::clNone)
{
  id = Figure::nextId++;
}

void Figure::buildRandomFigure()
{
  Type type = Type(rand() * TYPE_COUNT / (RAND_MAX + 1));
  buildFigure(type);
}

void Figure::buildFigure(Type type)
{
  char * cdata = NULL;
  color = Globals::Color::clNone;
  this->type = type;

  switch (type)
  {
  case typeI:
    haveSpecificRotation = true;
    dim = 4;
    color = Globals::Color::clCyan;
    cdata = "0000111100000000";
    break;
  case typeJ:
    haveSpecificRotation = false;
    dim = 3;
    color = Globals::Color::clBlue;
    cdata = "100111000";
    break;
  case typeL:
    haveSpecificRotation = false;
    dim = 3;
    color = Globals::Color::clOrange;
    cdata = "001111000";
    break;
  case typeO:
    haveSpecificRotation = false;
    dim = 2;
    color = Globals::Color::clYellow;
    cdata = "1111";
    break;
  case typeS:
    haveSpecificRotation = true;
    dim = 3;
    color = Globals::Color::clGreen;
    cdata = "011110000";
    break;
  case typeT:
    haveSpecificRotation = false;
    dim = 3;
    color = Globals::Color::clPurple;
    cdata = "010111000";
    break;
  case typeZ:
    haveSpecificRotation = true;
    dim = 3;
    color = Globals::Color::clRed;
    cdata = "110011000";
    break;
  default: 
    assert(0);
  }

  assert((int)strlen(cdata) == dim * dim);

  cells.clear();

  for (const char * ptr = cdata, *end = cdata + dim * dim; ptr < end; ptr++)
    cells.push_back(*ptr == '1' ? Cell(nextId, color) : Cell(0, Globals::Color::clNone));

  nextId++;
}

void Figure::internalRotateLeft()
{
  std::vector<Cell> curData = cells;

  for (int x = 0; x < dim; x++)
  for (int y = 0; y < dim; y++)
  {
    cells[x + y * dim] = curData[(dim - y - 1) + x * dim];
  }
}

void Figure::internalRotateRight()
{
  std::vector<Cell> curData = cells;

  for (int x = 0; x < dim; x++)
  for (int y = 0; y < dim; y++)
  {
    cells[x + y * dim] = curData[y + (dim - x - 1) * dim];
  }
}

void Figure::rotateLeft()
{
  if (haveSpecificRotation && specificRotatedFlag)
  {
    internalRotateRight();
    specificRotatedFlag = false;
  }
  else
  {
    internalRotateLeft();
    specificRotatedFlag = true;
  }
}

void Figure::rotateRight()
{
  if (haveSpecificRotation && !specificRotatedFlag)
  {
    internalRotateLeft();
    specificRotatedFlag = true;
  }
  else
  {
    internalRotateRight();
    specificRotatedFlag = false;
  }
}

Figure & Figure::operator = (const Figure & figure)
{
  haveSpecificRotation = figure.haveSpecificRotation;
  specificRotatedFlag = figure.specificRotatedFlag;
  type = figure.type;
  dim = figure.dim;
  color = figure.color;
  cells = figure.cells;
  return *this;
}

void Figure::swap(Figure & figure1, Figure & figure2)
{
  std::swap(figure1.cells, figure2.cells);

  bool tmp_haveSpecificRotation = figure1.haveSpecificRotation;
  figure1.haveSpecificRotation = figure2.haveSpecificRotation;
  figure2.haveSpecificRotation = tmp_haveSpecificRotation;
  
  bool tmp_specificRotatedFlag = figure1.specificRotatedFlag;
  figure1.specificRotatedFlag = figure2.specificRotatedFlag;
  figure2.specificRotatedFlag = tmp_specificRotatedFlag;

  Type tmp_type = figure1.type;
  figure1.type = figure2.type;
  figure2.type = tmp_type;

  int tmp_dim = figure1.dim;
  figure1.dim = figure2.dim;
  figure2.dim = tmp_dim;

  Globals::Color tmp_color = figure1.color;
  figure1.color = figure2.color;
  figure2.color = tmp_color;
}

void Figure::clear()
{
  haveSpecificRotation = false;
  specificRotatedFlag = false;
  type = typeNone;
  dim = 0;
  color = Globals::Color::clNone;
  cells.clear();
}
