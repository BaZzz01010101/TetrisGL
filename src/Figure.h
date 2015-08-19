#pragma once
#include "glall.h"
#include "Program.h"
#include "Shader.h"

class Figure
{
private:
  glm::vec3 color;
  std::vector<float> figureVertexBufferData;
  std::vector<float> figureUVBufferData;
  std::vector<int> borderMaskBufferData;
  GLuint figureVertexBufferId;
  GLuint figureUVBufferId;
  GLuint borderMaskBufferId;
  int vertCount;
  static GLuint textureId;
  static Program figureProg;
  static Shader figureVert;
  static Shader figureFrag;
  //static Program glowProg;
  //static Shader glowVert;
  //static Shader glowFrag;
  static glm::vec2 screenScale;

  void buildMeshes();

public:
  int dim;
  std::vector<int> data;
  static const glm::vec3 clRed;
  static const glm::vec3 clOrange;
  static const glm::vec3 clYellow;
  static const glm::vec3 clGreen;
  static const glm::vec3 clCyan;
  static const glm::vec3 clBlue;
  static const glm::vec3 clPurple;

  Figure(int dim, glm::vec3 color, const char * data);
  ~Figure();

  static void init();
  static void setScreenScale(const glm::vec2 & scale);
  void rotate(int halfPiAngles);
  void draw(float x, float y, float sqSize);
};

