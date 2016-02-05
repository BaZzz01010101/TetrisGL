#pragma once
#include "Cell.h"
#include "CellArray.h"

class Field : public CellArray
{
public:
  static const int width = 10;
  static const int height = 20;

  int getWidth() const { return width; }
  int getHeight() const { return height; }
  bool inBounds(int x, int y) const;
  Cell * getCell(int x, int y);
  const Cell * getCell(int x, int y) const { return const_cast<Field *>(this)->getCell(x, y); }
  void setCell(const Cell & cell, int x, int y);
  void clear();
  void copyRow(int srcRow, int dstRow);
  void clearRows(int beginRow, int endRow);

private:
  Cell cells[width * height];
};

