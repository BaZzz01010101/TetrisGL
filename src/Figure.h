#pragma once
#include "Cell.h"

class Figure
{
private:
  static int nextId;
  bool haveSpecificRotation;
  bool specificRotatedFlag;
  void internalRotateLeft();
  void internalRotateRight();

public:
  enum Type { typeNone = -1, typeI, typeJ, typeL, typeO, typeS, typeT, typeZ, TYPE_COUNT };
  int id;
  Type type;
  int dim;
  int angle;
  Cell::Color color;
  static const int dimMax = 4;
  Cell cells[dimMax * dimMax];

  Figure();

  void buildFigure(Type type);
  void buildRandomFigure();
  void rotateLeft();
  void rotateRight();
  Figure & operator = (const Figure & figure);
  static void swap(Figure & figure1, Figure & figure2);
  void clear();
};

