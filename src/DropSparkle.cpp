#include "static_headers.h"

#include "DropSparkle.h"
#include "Globals.h"

const float DropSparkle::minAlpha = 0.5f;
const float DropSparkle::maxAlpha = 1.0f;
const float DropSparkle::minSpeed = 3.0f;
const float DropSparkle::maxSpeed = 5.0f;

DropSparkle::DropSparkle()
{
  relX = float(fastrand()) / FAST_RAND_MAX;
  relY = float(fastrand()) / FAST_RAND_MAX;
  alpha = minAlpha + (maxAlpha - minAlpha) * float(fastrand()) / FAST_RAND_MAX;
  speed = minSpeed + (maxSpeed - minSpeed) * float(fastrand()) / FAST_RAND_MAX;
}


