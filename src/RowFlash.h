#pragma once
class RowFlash
{
private:
  double startTimer;
public:
  static const float effectTime;
  int y;

  RowFlash();
  float getProgress();
};

