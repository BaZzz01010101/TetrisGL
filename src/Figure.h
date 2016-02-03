#pragma once
#include "Cell.h"

class Figure
{
public:
  enum Type 
  { 
    typeNone = -1, 
    typeI, 
    typeJ, 
    typeL, 
    typeO, 
    typeS, 
    typeT, 
    typeZ, 
    TYPE_COUNT 
  };

  static const int dimMax = 4;
  int id;
  Type type;
  int dim;
  int angle;
  Cell::Color color;

  Figure();

  void build(Type type);
  void buildRandom();
  void rotateLeft();
  void rotateRight();
  static void swap(Figure & figure1, Figure & figure2);
  void clear();
  const Cell * getCell(int x, int y) const;

private:
  static int nextId;
  bool haveSpecificRotation;
  bool specificRotatedFlag;
  Cell cells[dimMax * dimMax];

  void internalRotateLeft();
  void internalRotateRight();
};
