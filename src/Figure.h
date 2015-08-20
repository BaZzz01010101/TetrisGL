#pragma once
#include "glall.h"
#include "Program.h"
#include "Shader.h"

class Figure
{
public:
  enum Color { clRed = 0, clOrange, clYellow, clGreen, clCyan, clBlue, clPurple };

private:
  Color color;
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
  void addVertex(float x, float y, float dx, float dy, float dv);

public:
  int dim;
  std::vector<int> data;

  Figure(int dim, Color color, const char * data);
  ~Figure();

  static void init();
  static void setScreenScale(const glm::vec2 & scale);
  void rotate(int halfPiAngles);
  void draw(float x, float y, float sqSize);
};

