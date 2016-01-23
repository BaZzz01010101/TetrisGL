#pragma once

class DropSparkle
{
private:
  static const float minAlpha;
  static const float maxAlpha;
  static const float minSpeed;
  static const float maxSpeed;

public:
  float relX;
  float relY;
  float alpha;
  float speed;

  DropSparkle();
};
