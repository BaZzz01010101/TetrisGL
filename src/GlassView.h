#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Model.h"
#include "Program.h"
#include "Shader.h"

class GlassView
{
private:
  Model & model;
  std::vector<float> vertexBuffer;
  GLuint vertexBufferId;
  Program figureProg;
  Shader figureVert;
  Shader figureFrag;
  int vertexCount;
  Cell * getGlassCell(int x, int y);
  void addVertex(float x, float y, const glm::vec2 & uv, int texIndex, const glm::vec3 & color, float alpha);
  static glm::vec3 blockColors[7];

public:
  GlassView(Model & model);
  ~GlassView();

  void init();
  void rebuildMesh();
  void draw();
};
