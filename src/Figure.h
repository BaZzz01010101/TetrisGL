#pragma once
#include "Cell.h"
#include "CellArray.h"

class Figure : public CellArray
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
  int getWidth() const { return dim; }
  int getHeight() const { return dim; }
  bool inBounds(int x, int y) const;
  Cell * getCell(int x, int y);
  const Cell * getCell(int x, int y) const { return const_cast<Figure *>(this)->getCell(x, y); }
  void setCell(const Cell & cell, int x, int y);
  glm::vec2 getCenterPos() const;

private:
  static int nextId;
  bool haveSpecificRotation;
  bool specificRotatedFlag;
  Cell cells[dimMax * dimMax];

  void internalRotateLeft();
  void internalRotateRight();
};
