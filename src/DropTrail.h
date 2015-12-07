#pragma once
#include "Globals.h"
#include "DropSparkle.h"
#include "Cell.h"

class DropTrail
{
private:
  double dropTimer;

public:
  static const int sparkleQty = 200;
  static const float trailEffectTime;
  static const float sparklesEffectTime;
  int x;
  int y;
  int height;
  Cell::Color color;
  DropSparkle sparkles[sparkleQty];

  DropTrail();
  float getTrailProgress() const;
  float getSparklesProgress() const;
};

