#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Model.h"
#include "Program.h"
#include "Shader.h"
#include "MainMesh.h"

class View
{
private:
  Model & model;
  GLuint vaoId;
  MainMesh mainMesh;

  Program figureProg;
  Shader figureVert;
  Shader figureFrag;
  
  GLuint glassFigureVertexBufferId;
  GLuint glassFigureUVWBufferId;
  GLuint glassShadowVertexBufferId;
  GLuint glassShadowUVWBufferId;
  GLuint glassGlowVertexBufferId;
  GLuint glassGlowAlphaBufferId;

  static std::vector<float> glassCellsVertexBuffer;
  GLuint glassCellsVertexBufferId;

  int glassFigureVertCount;
  int glassShadowVertCount;
  int glassGlowVertCount;

  void fillAdjacentCellsArray(Cell * cells, int cellX, int cellY, int cellsWidth, int cellsHeight, Cell * (&adjCells)[3][3]);
  void buildGlassMeshes();
  void buildMeshes(Cell * cells, int cellsWidth, int cellsHeight, GLuint figureVertexBufferId, GLuint figureUVWBufferId, GLuint glowVertexBufferId, GLuint glowAlphaBufferId, int * figureVertCount, int * glowVertCount);
  int addCellVertices(int cellX, int cellY, const Cell & cell, Cell * (&adjCells)[3][3], std::vector<float> & vertexBuffer, std::vector<float> & uvwBuffer);
  int addCellShadowVertices(float cellX, float cellY, const Cell & cell, Cell * (&adjCells)[3][3], std::vector<float> & vertexBuffer, std::vector<float> & uvwBuffer);
  //int addCellShadowMesh(int cellX, int cellY, Cell & cell, Cell * (&adjCells)[3][3], std::vector<float> vertexBuffer, std::vector<float> uvwBuffer);
  //int addCellGlowMesh(int cellX, int cellY, Cell & cell, Cell * (&adjCells)[3][3], std::vector<float> vertexBuffer, std::vector<float> alphaBuffer);


public:
  View(Model & model);
  ~View();

  void init();
  void update();
};

