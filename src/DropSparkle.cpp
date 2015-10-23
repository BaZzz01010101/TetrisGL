#include "DropSparkle.h"
#include "Globals.h"

DropSparkle::DropSparkle()
{
  relX = float(fastrand()) / FAST_RAND_MAX;
  relY = float(fastrand()) / FAST_RAND_MAX;
  alpha = 0.5f + float(fastrand()) / (FAST_RAND_MAX * 2);
  speed = 3.0f + float(fastrand()) / (FAST_RAND_MAX / 2 + 1);
}


