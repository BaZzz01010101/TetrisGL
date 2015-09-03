#include <assert.h>
#include <math.h>

#include "View.h"
#include "Crosy.h"
#include "Globals.h"
#include "3rdParty/SOIL2/SOIL2.h"

View::View(Model & model) :
  figureVert(GL_VERTEX_SHADER),
  figureFrag(GL_FRAGMENT_SHADER),
  model(model)
{
}


View::~View()
{
}

void View::init()
{
  const int initVertexCount = 4096;
  vertexBuffer.reserve(initVertexCount * 2);
  uvwBuffer.reserve(initVertexCount * 3);
  alphaBuffer.reserve(initVertexCount);

  glewExperimental = GL_TRUE;

  GLboolean glewInitResult = glewInit();
  assert(glewInitResult == GLEW_OK);

  // workaround GLEW issue with GL_INVALID_ENUM rising just after glewInit
  glGetError();

  std::string backPath = Crosy::getExePath() + "\\textures\\blocks.png";
  int width, height, channels;
  unsigned char * img = SOIL_load_image(backPath.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
  assert(width == Globals::mainArrayTextureSize);
  assert(!(height % Globals::mainArrayTextureSize));

  glGenTextures(1, &Globals::mainArrayTextureId);
  assert(!checkGlErrors());
  glBindTexture(GL_TEXTURE_2D_ARRAY, Globals::mainArrayTextureId);
  assert(!checkGlErrors());
  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, Globals::mainArrayTextureSize, Globals::mainArrayTextureSize, height / Globals::mainArrayTextureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
  assert(!checkGlErrors());
  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
  assert(!checkGlErrors());
  SOIL_free_image_data(img);

  glGenVertexArrays(1, &vaoId);
  assert(!checkGlErrors());

  glBindVertexArray(vaoId);
  assert(!checkGlErrors());

  glGenBuffers(1, &glassFigureVertexBufferId);
  assert(!checkGlErrors());

  glGenBuffers(1, &glassFigureUVWBufferId);
  assert(!checkGlErrors());

  glGenBuffers(1, &glassFigureVertexBufferId);
  assert(!checkGlErrors());

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  //assert(!checkGlErrors());

  glDisable(GL_DEPTH_TEST);
  assert(!checkGlErrors());

  glEnable(GL_BLEND);
  assert(!checkGlErrors());

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  assert(!checkGlErrors());

  glDisable(GL_CULL_FACE);
  assert(!checkGlErrors());

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  assert(!checkGlErrors());

  glClearDepth(1.0f);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_LOD_BIAS, -1);
  assert(!checkGlErrors());

  figureVert.compileFromString(
    "#version 330 core\n"
    "layout(location = 0) in vec2 vertexPos;"
    "layout(location = 1) in vec3 vertexUVW;"
    "uniform float scale;"
    "uniform vec2 pos;"
    "out vec3 uvw;"
  
    "void main()"
    "{"
    "  gl_Position = vec4(vertexPos * scale + pos, 0, 1);"
    "  uvw = vertexUVW;"
    "}");
  
  figureFrag.compileFromString(
    "#version 330 core\n"
    "uniform sampler2DArray tex;"
    "in vec3 uvw;"
    "out vec4 out_color;"
  
    "void main()"
    "{"
    "  out_color = texture(tex, uvw).rgba;"
    "}");
  
  figureProg.attachShader(figureVert);
  figureProg.attachShader(figureFrag);
  figureProg.link();
  figureProg.use();
  figureProg.setUniform("tex", 0);

}

void View::update()
{
  if (model.curFigureChanged)
  {
//    model.curFigureChanged = false;
  }
  
  if (model.glassChanged)
  {
    buildGlassMeshes();
//    model.glassChanged = false;
  }

  draw();
}

void View::fillAdjacentCellsArray(Cell * cells, int cellX, int cellY, int cellsWidth, int cellsHeight, Cell * (&adjCells)[3][3])
{
  for (int i = 0; i < 4; i++)
  {
    int cornerDX = (i & 1) * 2 - 1;
    int cornerDY = (i & 2) - 1;
    int sideDX = (i & 1) ? 0 : (i & 2) - 1;
    int sideDY = (i & 1) ? (i & 2) - 1 : 0;

    int cornerX = cellX + cornerDX;
    int cornerY = cellY + cornerDY;
    int sideX = cellX + sideDX;
    int sideY = cellY + sideDY;

    bool cornerInBounds = cornerX >= 0 && cornerX < cellsWidth && cornerY >= 0 && cornerY < cellsHeight;
    bool sideInBounds = sideX >= 0 && sideX < cellsWidth && sideY >= 0 && sideY < cellsHeight;

    adjCells[sideDX][sideDY] = sideInBounds ? cells + sideX + sideY * cellsWidth : NULL;
    adjCells[cornerDX][cornerDY] = cornerInBounds ? cells + cornerX + cornerY * cellsWidth : NULL;
  }
}

int View::addCellVertices(float originX, float originY, int cellX, int cellY, const Cell & cell, Cell * (&adjCells)[3][3], std::vector<float> & vertexBuffer, std::vector<float> & uvwBuffer)
{
  const float pixHalfSize = Globals::mainArrayTexturePixelSize / 2.0f;
  const int curCellId = cell.figureId;
  int vertCount = 0;

  for (int i = 0; i < 4; i++)
  {
    float vertexDX = float(i & 1);
    float vertexDY = float((i & 2) >> 1);
    float textureU = glm::clamp(vertexDX, pixHalfSize, 1.0f - pixHalfSize);
    float textureV = glm::clamp(vertexDY, pixHalfSize, 1.0f - pixHalfSize);

    int cornerDX = (i & 1) * 2 - 1;
    int cornerDY = (i & 2) - 1;

    bool haveHorzAdjCell = (adjCells[cornerDX][0] && adjCells[cornerDX][0]->figureId == curCellId);
    bool haveVertAdjCell = (adjCells[0][cornerDY] && adjCells[0][cornerDY]->figureId == curCellId);
    bool haveCornerAdjCell = (adjCells[cornerDX][cornerDY] && adjCells[cornerDX][cornerDY]->figureId == curCellId);


    int horzPolyTexIndexBase;
    int vertPolyTexIndexBase;

    if (haveHorzAdjCell && haveVertAdjCell && haveCornerAdjCell)
    {
      horzPolyTexIndexBase = Globals::openBlocksTexIndex;
      vertPolyTexIndexBase = Globals::openBlocksTexIndex;
    }
    else
    {
      if (haveHorzAdjCell)
        horzPolyTexIndexBase = Globals::horzBlocksTexIndex;
      else
        horzPolyTexIndexBase = Globals::vertBlocksTexIndex;

      if (haveVertAdjCell)
        vertPolyTexIndexBase = Globals::vertBlocksTexIndex;
      else
        vertPolyTexIndexBase = Globals::horzBlocksTexIndex;
    }

    vertexBuffer.push_back(originX + cellX + 0.5f);
    vertexBuffer.push_back(originY - cellY - 0.5f);
    vertexBuffer.push_back(originX + cellX + float(vertexDX));
    vertexBuffer.push_back(originY - cellY - float(vertexDY));
    vertexBuffer.push_back(originX + cellX + 0.5f);
    vertexBuffer.push_back(originY - cellY - float(vertexDY));

    vertexBuffer.push_back(originX + cellX + 0.5f);
    vertexBuffer.push_back(originY - cellY - 0.5f);
    vertexBuffer.push_back(originX + cellX + float(vertexDX));
    vertexBuffer.push_back(originY - cellY - float(vertexDY));
    vertexBuffer.push_back(originX + cellX + float(vertexDX));
    vertexBuffer.push_back(originY - cellY - 0.5f);

    float horzTexLayer = float(horzPolyTexIndexBase + cell.color);
    float vertTexLayer = float(vertPolyTexIndexBase + cell.color);

    uvwBuffer.push_back(0.5f);
    uvwBuffer.push_back(0.5f);
    uvwBuffer.push_back(vertTexLayer);

    uvwBuffer.push_back(textureU);
    uvwBuffer.push_back(textureV);
    uvwBuffer.push_back(vertTexLayer);

    uvwBuffer.push_back(0.5f);
    uvwBuffer.push_back(textureV);
    uvwBuffer.push_back(vertTexLayer);

    uvwBuffer.push_back(0.5f);
    uvwBuffer.push_back(0.5f);
    uvwBuffer.push_back(horzTexLayer);

    uvwBuffer.push_back(textureU);
    uvwBuffer.push_back(textureV);
    uvwBuffer.push_back(horzTexLayer);

    uvwBuffer.push_back(textureU);
    uvwBuffer.push_back(0.5f);
    uvwBuffer.push_back(horzTexLayer);

    vertCount += 6;
  }

  return vertCount;
}

void View::buildGlassMeshes()
{
  Cell * cells = model.glass.data();
  int glassWidth = model.glassWidth;
  int glassHeight = model.glassHeight;
  const float pixHalfSize = Globals::mainArrayTexturePixelSize / 2.0f;
  vertexBuffer.clear();
  uvwBuffer.clear();
  glassFigureVertCount = 0;
  Cell * buf[9];
  Cell * (&adjCells)[3][3] = (Cell * (&)[3][3])buf[4];
  int index = 0;

  for (int cellY = 0; cellY < glassHeight; cellY++)
  for (int cellX = 0; cellX < glassWidth; cellX++)
  {
    Cell & curCell = cells[index];
    int curCellId = curCell.figureId;

    if (curCell)
    {
      fillAdjacentCellsArray(cells, cellX, cellY, glassWidth, glassHeight, adjCells);
      glassFigureVertCount += addCellVertices(0, 0, cellX, cellY, curCell, adjCells, vertexBuffer, uvwBuffer);
    }

    index++;
  }

  if (model.curFigure)
  {
    Cell * curFigureCells = model.curFigure->cells.data();
    int curFigureX = model.curFigureX;
    int curFigureY = model.curFigureY;
    int curFigureDim = model.curFigure->dim;
    index = 0;

    for (int cellY = 0; cellY < curFigureDim; cellY++)
    for (int cellX = 0; cellX < curFigureDim; cellX++)
    {

      Cell & curCell = curFigureCells[index];

      if (curCell)
      {
        fillAdjacentCellsArray(curFigureCells, cellX, cellY, curFigureDim, curFigureDim, adjCells);
        glassFigureVertCount += addCellVertices(curFigureX, -curFigureY, cellX, cellY, curCell, adjCells, vertexBuffer, uvwBuffer);
      }

      index++;
    }
  }

  if (glassFigureVertCount)
  {
    glBindBuffer(GL_ARRAY_BUFFER, glassFigureVertexBufferId);
    assert(!checkGlErrors());

    glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), vertexBuffer.data(), GL_STATIC_DRAW);
    assert(!checkGlErrors());

    glBindBuffer(GL_ARRAY_BUFFER, glassFigureUVWBufferId);
    assert(!checkGlErrors());

    glBufferData(GL_ARRAY_BUFFER, uvwBuffer.size() * sizeof(float), uvwBuffer.data(), GL_STATIC_DRAW);
    assert(!checkGlErrors());
  }
}

void View::buildMeshes(Cell * cells, int cellsWidth, int cellsHeight, GLuint figureVertexBufferId, GLuint figureUVWBufferId, GLuint glowVertexBufferId, GLuint glowAlphaBufferId, int * figureVertCount, int * glowVertCount)
{
  static int figureBufVertCount = 4096;
  static std::vector<float> figureVertexBuf(figureBufVertCount * 2);
  static std::vector<float> figureUVWBuf(figureBufVertCount * 3);
  //figureVertexBufferData.clear();
  //figureUVBufferData.clear();
  //figureVertexBufferData.reserve(4096);
  //figureUVBufferData.reserve(4096);
  float * figureVertexBufferData = figureVertexBuf.data();
  float * figureUVWBufferData = figureUVWBuf.data();
  const float pixHalfSize = Globals::mainArrayTexturePixelSize / 2.0f;

  glassFigureVertCount = 0;

  int index = 0;
  
  for (int curCellY = 0; curCellY < cellsHeight; curCellY++)
  for (int curCellX = 0; curCellX < cellsWidth; curCellX++)
  {
    Cell & curCell = cells[index];
    int curCellId = curCell.figureId;

    Cell * buf[9] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    memset(buf, 0, sizeof(buf));
    Cell * (&adjCells)[3][3] = (Cell * (&)[3][3])buf[4];

    for (int i = 0; i < 4; i++)
    {
      int cornerDX = (i & 1) * 2 - 1;
      int cornerDY = (i & 2) - 1;
      int sideDX = (i & 1) ? 0 : (i & 2) - 1;
      int sideDY = (i & 1) ? (i & 2) - 1 : 0;

      int cornerX = curCellX + cornerDX;
      int cornerY = curCellY + cornerDY;
      int sideX = curCellX + sideDX;
      int sideY = curCellY + sideDY;

      bool cornerInBounds = cornerX >= 0 && cornerX < cellsWidth && cornerY >= 0 && cornerY < cellsHeight;
      bool sideInBounds = sideX >= 0 && sideX < cellsWidth && sideY >= 0 && sideY < cellsHeight;

      adjCells[sideDX][sideDY] = sideInBounds ? cells + sideX + sideY * cellsWidth : NULL;
      adjCells[cornerDX][cornerDY] = cornerInBounds ? cells + cornerX + cornerY * cellsWidth : NULL;
    }

    if (curCell)
    {
      for (int i = 0; i < 4; i++)
      {
        float vertexDX = float(i & 1);
        float vertexDY = float((i & 2) >> 1);
        float textureU = glm::clamp(vertexDX, pixHalfSize, 1.0f - pixHalfSize);
        float textureV = glm::clamp(vertexDY, pixHalfSize, 1.0f - pixHalfSize);

        int cornerDX = (i & 1) * 2 - 1;
        int cornerDY = (i & 2) - 1;

        int cornerX = curCellX + cornerDX;
        int cornerY = curCellY + cornerDY;

        bool dxInBounds = (cornerX >= 0 && cornerX < cellsWidth);
        bool dyInBounds = (cornerY >= 0 && cornerY < cellsHeight);

        bool haveHorzAdjCell = (adjCells[cornerDX][0] && adjCells[cornerDX][0]->figureId == curCellId);
        bool haveVertAdjCell = (adjCells[0][cornerDY] && adjCells[0][cornerDY]->figureId == curCellId);
        bool haveCornerAdjCell = (adjCells[cornerDX][cornerDY] && adjCells[cornerDX][cornerDY]->figureId == curCellId);


        int horzPolyTexIndexBase;
        int vertPolyTexIndexBase;

        if (haveHorzAdjCell && haveVertAdjCell && haveCornerAdjCell)
        {
          horzPolyTexIndexBase = Globals::openBlocksTexIndex;
          vertPolyTexIndexBase = Globals::openBlocksTexIndex;
        }
        else
        {
          if (haveHorzAdjCell)
            horzPolyTexIndexBase = Globals::horzBlocksTexIndex;
          else
            horzPolyTexIndexBase = Globals::vertBlocksTexIndex;

          if (haveVertAdjCell)
            vertPolyTexIndexBase = Globals::vertBlocksTexIndex;
          else
            vertPolyTexIndexBase = Globals::horzBlocksTexIndex;
        }

        if (glassFigureVertCount + 6 > figureBufVertCount)
        {
          figureBufVertCount *= 2;
          figureVertexBuf.resize(figureVertexBuf.size() * 2);
          figureUVWBuf.resize(figureUVWBuf.size() * 2);
          figureVertexBufferData = figureVertexBuf.data() + glassFigureVertCount * 2;
          figureUVWBufferData = figureUVWBuf.data() + glassFigureVertCount * 3;
        }

        glassFigureVertCount += 6;

        *figureVertexBufferData++ = curCellX + 0.5f;
        *figureVertexBufferData++ = -curCellY - 0.5f;
        *figureVertexBufferData++ = curCellX + float(vertexDX);
        *figureVertexBufferData++ = -curCellY - float(vertexDY);
        *figureVertexBufferData++ = curCellX + 0.5f;
        *figureVertexBufferData++ = -curCellY - float(vertexDY);

        *figureVertexBufferData++ = curCellX + 0.5f;
        *figureVertexBufferData++ = -curCellY - 0.5f;
        *figureVertexBufferData++ = curCellX + float(vertexDX);
        *figureVertexBufferData++ = -curCellY - float(vertexDY);
        *figureVertexBufferData++ = curCellX + float(vertexDX);
        *figureVertexBufferData++ = -curCellY - 0.5f;

        *figureUVWBufferData++ = 0.5f;
        *figureUVWBufferData++ = 0.5f;
        *figureUVWBufferData++ = float(vertPolyTexIndexBase + curCell.color);

        *figureUVWBufferData++ = textureU;
        *figureUVWBufferData++ = textureV;
        *figureUVWBufferData++ = float(vertPolyTexIndexBase + curCell.color);

        *figureUVWBufferData++ = 0.5f;
        *figureUVWBufferData++ = textureV;
        *figureUVWBufferData++ = float(vertPolyTexIndexBase + curCell.color);

        *figureUVWBufferData++ = 0.5f;
        *figureUVWBufferData++ = 0.5f;
        *figureUVWBufferData++ = float(horzPolyTexIndexBase + curCell.color);

        *figureUVWBufferData++ = textureU;
        *figureUVWBufferData++ = textureV;
        *figureUVWBufferData++ = float(horzPolyTexIndexBase + curCell.color);

        *figureUVWBufferData++ = textureU;
        *figureUVWBufferData++ = 0.5f;
        *figureUVWBufferData++ = float(horzPolyTexIndexBase + curCell.color);
      }
    }
    else
    {
      //if (glassFigureVertCount + 12 > figureBufVertCount)
      //{
      //  figureBufVertCount *= 2;
      //  figureVertexBuf.resize(figureVertexBuf.size() * 2);
      //  figureUVWBuf.resize(figureUVWBuf.size() * 2);
      //  figureVertexBufferData = figureVertexBuf.data() + glassFigureVertCount * 2;
      //  figureUVWBufferData = figureUVWBuf.data() + glassFigureVertCount * 3;
      //}

      //glassFigureVertCount += 12;

      //const float shadowWidth = 0.2f;

      //int leftAdjCellId = adjCells[-1][0] ? adjCells[-1][0]->figureId : 0;
      //int leftTopAdjCellId = adjCells[-1][-1] ? adjCells[-1][-1]->figureId : 0;
      //int topAdjCellId = adjCells[0][-1] ? adjCells[0][-1]->figureId : 0;

      //if (leftAdjCellId)
      //{
      //  float dy, v;

      //  if (leftTopAdjCellId == curCellId)
      //  {
      //    if (topAdjCellId == curCellId)
      //    {
      //      dy = -shadowWidth;
      //      v = 0.0f + pixHalfSize;
      //    }
      //    else
      //    {
      //      dy = 0.0f;
      //      v = 0.0f + pixHalfSize;
      //    }
      //  }
      //  else
      //  {
      //    dy = -shadowWidth;
      //    v = 1.0f - pixHalfSize;
      //  }

      //  *figureVertexBufferData++ = curCellX;
      //  *figureVertexBufferData++ = curCellY;
      //  *figureVertexBufferData++ = curCellX;
      //  *figureVertexBufferData++ = curCellY + 1.0f;
      //  *figureVertexBufferData++ = curCellX + shadowWidth;
      //  *figureVertexBufferData++ = curCellY + dy;

      //  *figureVertexBufferData++ = curCellX + shadowWidth;
      //  *figureVertexBufferData++ = curCellY + dy;
      //  *figureVertexBufferData++ = curCellX;
      //  *figureVertexBufferData++ = curCellY + 1.0f;

      //  *figureUVWBufferData++ = 0.0f + pixHalfSize;
      //  *figureUVWBufferData++ = v;
      //  *figureUVWBufferData++ = float(Globals::shadowTexIndex);

      //  *figureUVWBufferData++ = 0.0f + pixHalfSize;
      //  *figureUVWBufferData++ = 0.0f + pixHalfSize;
      //  *figureUVWBufferData++ = float(Globals::shadowTexIndex);

      //  *figureUVWBufferData++ = 1.0f - pixHalfSize;
      //  *figureUVWBufferData++ = v;
      //  *figureUVWBufferData++ = float(Globals::shadowTexIndex);
      //  
      //  int bottomLeftAdjCellId = adjCells[-1][1] ? adjCells[-1][1]->figureId : 0;
      //  int bottomAdjCellId = adjCells[0][1] ? adjCells[0][1]->figureId : 0;

      //  v = 0.0f + pixHalfSize;
      //  dy = (bottomAdjCellId || bottomLeftAdjCellId) ? 0.0f : -shadowWidth;

      //  *figureVertexBufferData++ = curCellX + shadowWidth;
      //  *figureVertexBufferData++ = curCellY + dy;

      //  if (bm & bmRightBottom)
      //  {
      //    addShadowVertex(fx + 1.0f + shadowWidth, fy - 1.0f - shadowWidth, bottomU, 1.0f);
      //    addShadowVertex(fx + 1.0f + shadowWidth, fy - 1.0f - shadowWidth, bottomU, 1.0f);
      //  }
      //  else
      //  {
      //    addShadowVertex(fx + 1.0f + shadowWidth, fy - 1.0f, bottomU, 1.0f);
      //    addShadowVertex(fx + 1.0f + shadowWidth, fy - 1.0f, bottomU, 1.0f);
      //  }

      //  addShadowVertex(fx + 1.0f, fy + 0.0f, topU, 0.0f);

      //  if (bm & bmTopRight)
      //    addShadowVertex(fx + 1.0f + shadowWidth, fy - shadowWidth, topU, 1.0f);
      //  else
      //    addShadowVertex(fx + 1.0f + shadowWidth, fy - 0.0f, topU, 1.0f);
      //}

      //if (bm & bmBottom)
      //{
      //  const float ru = 0.9f;
      //  float lu;

      //  if (bm & bmLeft)
      //    lu = 0.0f;
      //  else
      //    lu = 0.5f;

      //  addShadowVertex(fx + 0.0f, fy - 1.0f, lu, 0.0f);
      //  addShadowVertex(fx + 1.0f, fy - 1.0f, ru, 0.0f);

      //  if (bm & bmRightBottom)
      //  {
      //    addShadowVertex(fx + 1.0f + shadowWidth, fy - 1.0f - shadowWidth, ru, 1.0f);
      //    addShadowVertex(fx + 1.0f + shadowWidth, fy - 1.0f - shadowWidth, ru, 1.0f);
      //  }
      //  else
      //  {
      //    addShadowVertex(fx + 1.0f, fy - 1.0f - shadowWidth, ru, 1.0);
      //    addShadowVertex(fx + 1.0f, fy - 1.0f - shadowWidth, ru, 1.0);
      //  }

      //  addShadowVertex(fx + 0.0f, fy - 1.0f, lu, 0.0f);

      //  if (bm & bmBottomLeft)
      //    addShadowVertex(fx + shadowWidth, fy - 1.0f - shadowWidth, lu, 1.0);
      //  else
      //    addShadowVertex(fx + 0.0f, fy - 1.0f - shadowWidth, lu, 1.0f);
      //}

    }

    index++;
  }

  if (glassFigureVertCount)
  {
    glBindBuffer(GL_ARRAY_BUFFER, figureVertexBufferId);
    assert(!checkGlErrors());

    glBufferData(GL_ARRAY_BUFFER, glassFigureVertCount * 2 * sizeof(float), figureVertexBuf.data(), GL_STATIC_DRAW);
    assert(!checkGlErrors());

    glBindBuffer(GL_ARRAY_BUFFER, figureUVWBufferId);
    assert(!checkGlErrors());

    glBufferData(GL_ARRAY_BUFFER, glassFigureVertCount * 3 * sizeof(float), figureUVWBuf.data(), GL_STATIC_DRAW);
    assert(!checkGlErrors());
  }
}

void View::draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  assert(!checkGlErrors());

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  assert(!checkGlErrors());

  figureProg.use();
  figureProg.setUniform("pos", glm::vec2(-0.66, 1));
  figureProg.setUniform("scale", 0.1f);
  glEnableVertexAttribArray(0);
  assert(!checkGlErrors());
  glEnableVertexAttribArray(1);
  assert(!checkGlErrors());
  
  
  glBindBuffer(GL_ARRAY_BUFFER, glassFigureVertexBufferId);
  assert(!checkGlErrors());
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!checkGlErrors());

  glBindBuffer(GL_ARRAY_BUFFER, glassFigureUVWBufferId);
  assert(!checkGlErrors());
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!checkGlErrors());

  glDrawArrays(GL_TRIANGLES, 0, glassFigureVertCount);
  assert(!checkGlErrors());


  glDisableVertexAttribArray(0);
  assert(!checkGlErrors());
  glDisableVertexAttribArray(1);
  assert(!checkGlErrors());
}
