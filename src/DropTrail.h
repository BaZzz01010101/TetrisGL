#pragma once
#include "Globals.h"
#include "DropSparkle.h"
#include "Cell.h"

class DropTrail
{
private:
  static const float trailEffectTime;
  static const float sparklesEffectTime;

public:
  static const int sparkleQty = 200;
  float trailTimeLeft;
  float sparklesTimeLeft;
  int x;
  int y;
  int height;
  Cell::Color color;
  DropSparkle sparkles[sparkleQty];

  DropTrail();
  void set(int x, int y, int height, Cell::Color color);
  bool update(float timeDelta);
  float getTrailProgress() const;
  float getSparklesProgress() const;
};

