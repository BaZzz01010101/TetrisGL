#include <glm/glm.hpp>
#include "DropTrail.h"

const float DropTrail::trailEffectTime = 0.5f;
const float DropTrail::sparklesEffectTime = 1.0f;

DropTrail::DropTrail()
{
  dropTimer = getTimer();
}

float DropTrail::getTrailProgress()
{
  double time = getTimer();
  return glm::min<float>(float(time - dropTimer) / trailEffectTime, 1.0f);
}

float DropTrail::getSparklesProgress()
{
  double time = getTimer();
  return glm::min<float>(float(time - dropTimer) / sparklesEffectTime, 1.0f);
}
