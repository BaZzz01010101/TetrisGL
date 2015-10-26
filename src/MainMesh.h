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

  Cell * getGlassCell(int x, int y);
  Cell * getFigureCell(Figure & figure, int x, int y);
  void addVertex(const glm::vec2 & xy, const glm::vec2 & uv, int texIndex, const glm::vec3 & color, float alpha);
  void clear();
  void sendToDevice();
  void buildBackground();
  void buildFigureBackgroundsMesh();
  void buidGlassShadow();
  void buidGlassBlocks();
  void biuldGlassGlow();
  void buildFigureBlocks();
  void buildFigureGlow();
  void buildDropTrails();
  void buildRowFlashes();

public:
  MainMesh(Model & model);
  ~MainMesh();

  void init();
  void fillDepthBuffer();
  void rebuild();
  void draw();
};
