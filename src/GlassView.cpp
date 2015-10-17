#include "GlassView.h"
#include "Globals.h"


GlassView::GlassView(Model & model) :
figureVert(GL_VERTEX_SHADER),
figureFrag(GL_FRAGMENT_SHADER),
model(model)
{
}


GlassView::~GlassView()
{
}

void GlassView::init()
{
  glGenBuffers(1, &vertexBufferId);
  assert(!checkGlErrors());
  const int initBufferSize = 20480;
  vertexBuffer.reserve(initBufferSize);

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

Cell * GlassView::getGlassCell(int x, int y)
{
  if (x < 0 || y < 0 || x >= model.glassWidth || y >= model.glassHeight)
    return NULL;

  Cell * cell = model.glass.data() + x + y * model.glassWidth;

  if (!cell->figureId && model.curFigure)
  {
    bool isInCurFigure =
      x >= model.curFigureX &&
      x < model.curFigureX + model.curFigure->dim &&
      y >= model.curFigureY &&
      y < model.curFigureY + model.curFigure->dim;

    if (isInCurFigure)
      cell = model.curFigure->cells.data() + x - model.curFigureX + (y - model.curFigureY) * model.curFigure->dim;
  }

  return cell;
}

void GlassView::addVertex(float x, float y, float u, float v, float w)
{
  vertexBuffer.push_back(x);
  vertexBuffer.push_back(y);
  vertexBuffer.push_back(u);
  vertexBuffer.push_back(v);
  vertexBuffer.push_back(w);
  vertexCount ++;
}

void GlassView::rebuildMesh()
{
  const float pixSize = Globals::mainArrayTexturePixelSize;
  const float pixHalfSize = Globals::mainArrayTexturePixelSize / 2.0f;
  vertexCount = 0;
  vertexBuffer.clear();

  for (int y = 0; y < model.glassHeight; y++)
  for (int x = 0; x < model.glassWidth; x++)
  {
    Cell * cell = getGlassCell(x, y);

    if (cell && cell->figureId)
    {
      const int cellId = cell->figureId;

      for (int i = 0; i < 4; i++)
      {
        int cornerDX = (i & 1) * 2 - 1;
        int cornerDY = (i & 2) - 1;

        Cell * horzAdjCell = getGlassCell(x + cornerDX, y);
        Cell * vertAdjCell = getGlassCell(x, y + cornerDY);
        Cell * cornerAdjCell = getGlassCell(x + cornerDX, y + cornerDY);

        bool haveHorzAdjCell = (horzAdjCell && horzAdjCell->figureId == cellId);
        bool haveVertAdjCell = (vertAdjCell && vertAdjCell->figureId == cellId);
        bool haveCornerAdjCell = (cornerAdjCell && cornerAdjCell->figureId == cellId);

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

        const float horzTexLayer = float(horzPolyTexIndexBase + cell->color);
        const float vertTexLayer = float(vertPolyTexIndexBase + cell->color);
        const float dx = float(i & 1);
        const float dy = float((i & 2) >> 1);
        const float u = glm::clamp(dx, pixHalfSize, 1.0f - pixHalfSize);
        const float v = glm::clamp(dy, pixHalfSize, 1.0f - pixHalfSize);

        addVertex(x + 0.5f, -y - 0.5f, 0.5f, 0.5f, vertTexLayer);
        addVertex(x + dx,   -y - dy,   u,    v,    vertTexLayer);
        addVertex(x + 0.5f, -y - dy,   0.5f, v,    vertTexLayer);
        addVertex(x + 0.5f, -y - 0.5f, 0.5f, 0.5f, horzTexLayer);
        addVertex(x + dx,   -y - dy,   u,    v,    horzTexLayer);
        addVertex(x + dx,   -y - 0.5f, u,    0.5f, horzTexLayer);
      }

    }

    if (cell && !cell->figureId)
    {
      const float shadowTexLayer = float(Globals::shadowTexIndex);
      const float shadowWidth = 0.1f;

      Cell * leftCell = getGlassCell(x - 1, y);
      Cell * leftTopCell = getGlassCell(x - 1, y - 1);
      Cell * topCell = getGlassCell(x, y - 1);
      Cell * bottomLeftCell = getGlassCell(x - 1, y + 1);
      Cell * bottomCell = getGlassCell(x, y + 1);
      Cell * topRightCell = getGlassCell(x + 1, y - 1);
      Cell * rightCell = getGlassCell(x + 1, y);

      if (leftCell && leftCell->figureId)
      {
        float topDY, bottomDY;
        bool softTop = !leftTopCell || leftTopCell->figureId != leftCell->figureId;

        if (leftTopCell && leftTopCell->figureId == leftCell->figureId && topCell->figureId != leftCell->figureId)
          topDY = 0.0f;
        else
          topDY = shadowWidth;

        if ((bottomLeftCell && bottomLeftCell->figureId == leftCell->figureId) || !bottomCell || bottomCell->figureId)
          bottomDY = 0.0f;
        else
          bottomDY = shadowWidth;

        addVertex(x - pixSize, -y + pixSize, (softTop ? 1.0f - pixHalfSize : pixHalfSize), 0.5f, shadowTexLayer);
        addVertex(x + shadowWidth - pixSize, -y - topDY + pixSize, 1.0f - pixHalfSize, 0.5f, shadowTexLayer);
        addVertex(x - pixSize, -y - 1.0f + pixSize, pixHalfSize, 0.5f, shadowTexLayer);
        addVertex(x + shadowWidth - pixSize, -y - topDY + pixSize, 1.0f - pixHalfSize, 0.5f, shadowTexLayer);
        addVertex(x - pixSize, -y - 1.0f + pixSize, pixHalfSize, 0.5f, shadowTexLayer);
        addVertex(x + shadowWidth - pixSize, -y - 1.0f - bottomDY + pixSize, 1.0f - pixHalfSize, 0.5f, shadowTexLayer);
      }

      if (topCell && topCell->figureId)
      {
        float leftDX, rightDX;
        bool softLeft = !leftTopCell || leftTopCell->figureId != topCell->figureId;

        if (leftTopCell && leftTopCell->figureId == topCell->figureId && leftCell->figureId != topCell->figureId)
          leftDX = 0.0f;
        else
          leftDX = shadowWidth;

        if ((topRightCell && topRightCell->figureId == topCell->figureId) || !rightCell || rightCell->figureId)
          rightDX = 0.0f;
        else
          rightDX = shadowWidth;

        addVertex(x - pixSize, -y + pixSize, (softLeft ? 1.0f - pixHalfSize : pixHalfSize), 0.5f, shadowTexLayer);
        addVertex(x + leftDX - pixSize, -y - shadowWidth + pixSize, 1.0f - pixHalfSize, 0.5f, shadowTexLayer);
        addVertex(x + 1.0f - pixSize, -y + pixSize, pixHalfSize, 0.5f, shadowTexLayer);
        addVertex(x + leftDX - pixSize, -y - shadowWidth + pixSize, 1.0f - pixHalfSize, 0.5f, shadowTexLayer);
        addVertex(x + 1.0f - pixSize, -y + pixSize, pixHalfSize, 0.5f, shadowTexLayer);
        addVertex(x + 1.0f + rightDX - pixSize, -y - shadowWidth + pixSize, 1.0f - pixHalfSize, 0.5f, shadowTexLayer);
      }

      if (leftTopCell && leftTopCell->figureId && (!topCell || !topCell->figureId) && leftCell && leftCell->figureId && leftCell->figureId != leftTopCell->figureId)
      {
        addVertex(x - pixSize, -y + pixSize, pixHalfSize, 0.5f, shadowTexLayer);
        addVertex(x + shadowWidth - pixSize, -y - shadowWidth + pixSize, 1.0f - pixHalfSize, 0.5f, shadowTexLayer);
        addVertex(x - pixSize, -y - shadowWidth + pixSize, 1.0f - pixHalfSize, 0.5f, shadowTexLayer);
      }

      if (leftTopCell && leftTopCell->figureId && (!leftCell || !leftCell->figureId) && topCell && topCell->figureId && topCell->figureId != leftTopCell->figureId)
      {
        addVertex(x - pixSize, -y + pixSize, pixHalfSize, 0.5f, shadowTexLayer);
        addVertex(x + shadowWidth - pixSize, -y - shadowWidth + pixSize, 1.0f - pixHalfSize, 0.5f, shadowTexLayer);
        addVertex(x + shadowWidth - pixSize, -y + pixSize, 1.0f - pixHalfSize, 0.5f, shadowTexLayer);
      }
    }
  }

  if (vertexCount)
  {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    assert(!checkGlErrors());

    glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), vertexBuffer.data(), GL_STATIC_DRAW);
    assert(!checkGlErrors());
  }

}

void GlassView::draw()
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
  
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  assert(!checkGlErrors());
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  assert(!checkGlErrors());
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
  assert(!checkGlErrors());

  glDrawArrays(GL_TRIANGLES, 0, vertexCount);
  assert(!checkGlErrors());
  
  glDisableVertexAttribArray(0);
  assert(!checkGlErrors());
  glDisableVertexAttribArray(1);
  assert(!checkGlErrors());
}
