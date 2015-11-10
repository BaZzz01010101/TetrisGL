#pragma once
#include "Globals.h"

class Cell
{
public:
  int figureId;
  Globals::Color color;

  Cell();
  Cell(int figureId, Globals::Color color);

  void clear();
  Cell & operator = (const Cell & cell);
  inline bool isEmpty() const { return color == Globals::Color::clNone; };
};

