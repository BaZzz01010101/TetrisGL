#include <glm/glm.hpp>
#include "RowFlash.h"
#include "Globals.h"

const float RowFlash::effectTime = 0.4f;

RowFlash::RowFlash()
{
  startTimer = getTimer();
}

float RowFlash::getProgress()
{
  double time = getTimer();
  return glm::min<float>(float(time - startTimer) / effectTime, 1.0f);
}


