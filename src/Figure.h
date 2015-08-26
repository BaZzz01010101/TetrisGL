#pragma once
#include "glall.h"
#include "Program.h"
#include "Shader.h"

class Figure
{
public:
  enum Color { clRed = 0, clOrange, clYellow, clGreen, clCyan, clBlue, clPurple };

private:
  glm::vec2 pos;
  Color color;
  std::vector<float> figureVertexBufferData;
  std::vector<float> figureUVBufferData;
  std::vector<float> shadowVertexBufferData;
  std::vector<float> shadowUVBufferData;
  std::vector<float> glowVertexBufferData;
  std::vector<float> glowUVBufferData;
  std::vector<float> glowAlphaBufferData;
  GLuint figureVertexBufferId;
  GLuint figureUVWBufferId;
  GLuint shadowVertexBufferId;
  GLuint shadowUVBufferId;
  GLuint glowVertexBufferId;
  GLuint glowUVBufferId;
  GLuint glowAlphaBufferId;
  int figureVertCount;
  int shadowVertCount;
  int glowVertCount;
  static Program figureProg;
  static Shader figureVert;
  static Shader figureFrag;
  static Program glowProg;
  static Shader glowVert;
  static Shader glowFrag;

  void buildMeshes();
  void clearVertexBuffersData();
  void addFigureVertex(float sqx, float sqy, float u, float v, int blockArrayIndex);
  void addShadowVertex(float x, float y, float u, float v);
  void addGlowVertex(float x, float y, float alpha);

public:
  int dim;
  std::vector<int> data;

  Figure(int dim, Color color, const char * data);
  ~Figure();

  static void init();
  static void setScreen(const glm::vec2 & screen);
  static void setScale(float scale);

  inline void setPos(float x, float y) { this->pos = glm::vec2(x, y); }
  inline void setPos(const glm::vec2 & pos) { this->pos = pos; }
  inline glm::vec2 getPos() { return pos; };

  void rotate(int halfPiAngles);
  void drawFigure();
  void drawShadow();
  void drawGlow();
};

