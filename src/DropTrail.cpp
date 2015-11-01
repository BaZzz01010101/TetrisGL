#include "static_headers.h"

#include "DropTrail.h"

const float DropTrail::trailEffectTime = 0.6f;
const float DropTrail::sparklesEffectTime = 1.1f;

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
