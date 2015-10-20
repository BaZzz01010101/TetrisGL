#pragma once
#include "Globals.h"

class Cell
{
public:
  int figureId;
  Globals::Color color;

  Cell();
  Cell(int figureId, Globals::Color color);

  inline operator bool() { return color != Globals::Color::clNone; };
};

