#pragma once
#include "Model.h"
#include "MainMesh.h"

class View
{
private:
  Model & model;
  GLuint vaoId;
  MainMesh mainMesh;

public:
  View(Model & model);
  ~View();

  void init(int winWidth, int winHeight);
  void resize(int width, int height);
  void update();
};

