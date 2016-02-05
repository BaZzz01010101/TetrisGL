#pragma once
#include "Cell.h"

class CellArray
{
public:
  virtual int getWidth() const = 0;
  virtual int getHeight() const = 0;
  virtual bool inBounds(int x, int y) const = 0;
  virtual Cell * getCell(int x, int y) = 0;
  virtual const Cell * getCell(int x, int y) const = 0;
  virtual void setCell(const Cell & cell, int x, int y) = 0;
};

