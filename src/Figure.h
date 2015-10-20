#pragma once
#include <assert.h>
#include <vector>

#include "Cell.h"
//#include "Program.h"
//#include "Shader.h"

class Figure
{
private:
  static int nextId;

public:
  enum Type { typeNone = -1, typeI, typeJ, typeL, typeO, typeS, typeT, typeZ, TYPE_COUNT };
  enum Rotation { rotLeft, rotRight };
  int id;
  Type type;
  int dim;
  Globals::Color color;
  std::vector<Cell> cells;

  Figure();

  void buildFigure(Type type);
  void buildRandomFigure();
  void rotate(Rotation rot);
  Figure & operator = (const Figure & figure);
  static void swap(Figure & figure1, Figure & figure2);
  void clear();
};

