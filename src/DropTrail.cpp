#include "static_headers.h"

#include "DropTrail.h"
#include "Time.h"

const float DropTrail::trailEffectTime = 0.6f;
const float DropTrail::sparklesEffectTime = 0.8f;

DropTrail::DropTrail()
{
  trailTimeLeft = -1.0;
  sparklesTimeLeft = -1.0;
}


void DropTrail::set(int x, int y, int height, Cell::Color color)
{
  trailTimeLeft = trailEffectTime;
  sparklesTimeLeft = sparklesEffectTime;
  this->x = x;
  this->y = y;
  this->height = height;
  this->color = color;
}


bool DropTrail::update(float timeDelta)
{
  if (trailTimeLeft >= 0.0f)
    trailTimeLeft -= timeDelta;

  if (sparklesTimeLeft >= 0.0f)
    sparklesTimeLeft -= timeDelta;

  return trailTimeLeft > 0.0f || sparklesTimeLeft > 0.0f;
}


float DropTrail::getTrailProgress() const
{
  return glm::clamp(1.0f - trailTimeLeft / trailEffectTime, 0.0f, 1.0f);
}


float DropTrail::getSparklesProgress() const
{
  return glm::clamp(1.0f - sparklesTimeLeft / sparklesEffectTime, 0.0f, 1.0f);
}
