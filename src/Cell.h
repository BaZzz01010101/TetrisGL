#pragma once
#include "Globals.h"

class Cell
{
public:
  int figureId;
  enum Color { clNone = -1, clRed, clOrange, clYellow, clGreen, clCyan, clBlue, clPurple, COLOR_COUNT };
  Color color;

  Cell();
  Cell(int figureId, Color color);

  void clear();
  Cell & operator = (const Cell & cell);
  inline bool isEmpty() const { return color == Color::clNone; };
};

