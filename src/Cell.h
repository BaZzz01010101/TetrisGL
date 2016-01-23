#pragma once
#include "Globals.h"

class Cell
{
public:
  enum Color 
  { 
    clNone = -1, 
    clRed, 
    clOrange, 
    clYellow, 
    clGreen, 
    clCyan, 
    clBlue, 
    clPurple, 
    COLOR_COUNT 
  };

  int figureId;
  Color color;

  Cell();
  Cell(int figureId, Color color);
  Cell & operator = (const Cell & cell);
  void clear();
  inline bool isEmpty() const { return color == Color::clNone; };
};

