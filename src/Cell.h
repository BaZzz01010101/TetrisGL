#pragma once

class Cell
{
public:
  enum Color { clNone = -1, clRed, clOrange, clYellow, clGreen, clCyan, clBlue, clPurple };
  int figureId;
  Color color;

  Cell();
  Cell(int figureId, Color color);

  inline operator bool() { return color != clNone; };
};

