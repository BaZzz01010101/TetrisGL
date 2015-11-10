#include "static_headers.h"

#include "DropTrail.h"
#include "Time.h"

const float DropTrail::trailEffectTime = 0.6f;
const float DropTrail::sparklesEffectTime = 1.1f;

DropTrail::DropTrail()
{
  dropTimer = Time::timer;
}

float DropTrail::getTrailProgress() const
{
  double time = Time::timer;
  return glm::min<float>(float(time - dropTimer) / trailEffectTime, 1.0f);
}

float DropTrail::getSparklesProgress() const
{
  double time = Time::timer;
  return glm::min<float>(float(time - dropTimer) / sparklesEffectTime, 1.0f);
}
