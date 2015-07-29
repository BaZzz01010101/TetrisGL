#pragma once
class Game
{
public:
  Game();
  ~Game();

  void Pulse();
  void onKeyClick(int key, int scancode, int action, int mods);
  void onMouseClick(int key, int action, int mods);
  void onMouseMove(double xpos, double ypos);
};

