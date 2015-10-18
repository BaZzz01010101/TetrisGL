#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Model.h"
#include "Program.h"
#include "Shader.h"

class MainMesh
{
private:
  Model & model;
  std::vector<float> vertexBuffer;
  GLuint vertexBufferId;
  Program figureProg;
  Shader figureVert;
  Shader figureFrag;
  int vertexCount;
  glm::vec2 origin;
  float scale;
  Cell * getGlassCell(int x, int y);
  void addVertex(const glm::vec2 & xy, const glm::vec2 & uv, int texIndex, const glm::vec3 & color, float alpha);
  static glm::vec3 blockColors[7];
  void buildBackgroundMesh();
  void buildGlassBackgroundMesh();
  void buidGlassShadowMesh();
  void buidGlassBlocksMesh();
  void biuldGlassGlowMesh();

public:
  MainMesh(Model & model);
  ~MainMesh();

  void init();
  void rebuild();
  void draw();
};
