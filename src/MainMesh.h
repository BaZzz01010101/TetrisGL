#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Model.h"
#include "Program.h"
#include "Shader.h"

class MainMesh
{
private:
  std::vector<float> vertexBuffer;
  GLuint vertexBufferId;
  Program shaderProg;
  Shader shaderVert;
  Shader shaderFrag;
  int vertexCount;

public:
  MainMesh();
  ~MainMesh();

  void init();
  void clear();
  void addVertex(float x, float y, float u, float v);
  void send();
  void draw(float x, float y, float scale);
};

