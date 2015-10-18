#define _USE_MATH_DEFINES
#include <math.h>
#include "MainMesh.h"
#include "Globals.h"


MainMesh::MainMesh(Model & model) :
  figureVert(GL_VERTEX_SHADER),
  figureFrag(GL_FRAGMENT_SHADER),
  origin(0.0f),
  scale(0.0f),
  model(model)
{
}


MainMesh::~MainMesh()
{
}

glm::vec3 MainMesh::blockColors[7] = 
{
  { 1.00f, 0.05f, 0.05f },
  { 1.00f, 0.35f, 0.00f },
  { 0.85f, 0.60f, 0.00f },
  { 0.05f, 0.55f, 0.10f },
  { 0.05f, 0.60f, 1.00f },
  { 0.10f, 0.20f, 0.90f },
  { 0.40f, 0.10f, 0.80f }
};


void MainMesh::init()
{
  glGenBuffers(1, &vertexBufferId);
  assert(!checkGlErrors());
  const int initBufferSize = 20480;
  vertexBuffer.reserve(initBufferSize);

  figureVert.compileFromString(
    "#version 330 core\n"
    "layout(location = 0) in vec2 vertexPos;"
    "layout(location = 1) in vec3 vertexUVW;"
    "layout(location = 2) in vec4 vertexRGBA;"
    "uniform float scale;"
    "uniform vec2 pos;"
    "out vec3 uvw;"
    "out vec4 color;"
    "out vec2 pixPos;"

    "void main()"
    "{"
    "  gl_Position = vec4(vertexPos, 0, 1);"
    "  uvw = vertexUVW;"
    "  color = vertexRGBA;"
    "  pixPos = (vertexPos + vec2(-0.5, 0.5)) * 2.0;"
    "}");

  figureFrag.compileFromString(
    "#version 330 core\n"
    "uniform sampler2DArray tex;"
    "in vec3 uvw;"
    "in vec4 color;"
    "in vec2 pixPos;"
    "out vec4 out_color;"

    "float rand(vec2 co){"
    "  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);"
    "}"

    "void main()"
    "{"
    "  vec4 texcol = texture(tex, uvw).rgba;"
    "  float alpha = texcol.a * color.a;"
    "  vec3 rgb = mix(texcol.r * color.rgb, texcol.rrr, texcol.g);"
    "  out_color = vec4(rgb + (2.0f - rand(pixPos)) / 150, alpha);"
    "}");

  figureProg.attachShader(figureVert);
  figureProg.attachShader(figureFrag);
  figureProg.link();
  figureProg.use();
  figureProg.setUniform("tex", 0);
}

void MainMesh::rebuild()
{
  vertexCount = 0;
  vertexBuffer.clear();

  origin = Globals::gameBkPos;
  scale = 1.0f;
  buildBackgroundMesh();
  origin = Globals::glassPos;
  scale = 1.0f;
  buildGlassBackgroundMesh();
  origin = Globals::glassPos;
  scale = Globals::glassSize.x / 10.0f;
  buidGlassShadowMesh();
  buidGlassBlocksMesh();
  biuldGlassGlowMesh();

  if (vertexCount)
  {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    assert(!checkGlErrors());

    glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), vertexBuffer.data(), GL_STATIC_DRAW);
    assert(!checkGlErrors());
  }
}

void MainMesh::draw()
{
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  assert(!checkGlErrors());

  figureProg.use();
  glEnableVertexAttribArray(0);
  assert(!checkGlErrors());
  glEnableVertexAttribArray(1);
  assert(!checkGlErrors());
  glEnableVertexAttribArray(2);
  assert(!checkGlErrors());

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  assert(!checkGlErrors());
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
  assert(!checkGlErrors());
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(2 * sizeof(float)));
  assert(!checkGlErrors());
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));
  assert(!checkGlErrors());

  glDrawArrays(GL_TRIANGLES, 0, vertexCount);
  assert(!checkGlErrors());
  
  glDisableVertexAttribArray(0);
  assert(!checkGlErrors());
  glDisableVertexAttribArray(1);
  assert(!checkGlErrors());
  glDisableVertexAttribArray(2);
  assert(!checkGlErrors());
}

Cell * MainMesh::getGlassCell(int x, int y)
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

void MainMesh::addVertex(const glm::vec2 & xy, const glm::vec2 & uv, int texIndex, const glm::vec3 & color, float alpha)
{
  vertexBuffer.push_back(origin.x + xy.x * scale);
  vertexBuffer.push_back(origin.y + xy.y * scale);
  vertexBuffer.push_back(uv.x);
  vertexBuffer.push_back(uv.y);
  //vertexBuffer.push_back(glm::clamp(uv.x, Globals::mainArrayTexturePixelSize / 2.0f, 1.0f - Globals::mainArrayTexturePixelSize / 2.0f));
  //vertexBuffer.push_back(glm::clamp(uv.y, Globals::mainArrayTexturePixelSize / 2.0f, 1.0f - Globals::mainArrayTexturePixelSize / 2.0f));
  vertexBuffer.push_back(float(texIndex));
  vertexBuffer.push_back(color.r);
  vertexBuffer.push_back(color.g);
  vertexBuffer.push_back(color.b);
  vertexBuffer.push_back(alpha);
  vertexCount++;
}

void MainMesh::buildBackgroundMesh()
{
  const int xfr = 6;
  const int yfr = 6;

  for (int y = 0; y < yfr; y++)
  for (int x = 0; x < xfr; x++)
  {
    glm::vec2 verts[4] =
    {
      { Globals::gameBkSize.x * x / xfr,       -Globals::gameBkSize.y * y / yfr },
      { Globals::gameBkSize.x * x / xfr,       -Globals::gameBkSize.y * (y + 1) / yfr },
      { Globals::gameBkSize.x * (x + 1) / xfr, -Globals::gameBkSize.y * y / yfr },
      { Globals::gameBkSize.x * (x + 1) / xfr, -Globals::gameBkSize.y * (y + 1) / yfr },
    };

    //const glm::vec2 glassLeftTop = Globals::glassPos - Globals::gameBkPos;
    //const glm::vec2 glassRightBottom = glm::vec2(glassLeftTop.x + Globals::glassSize.x, glassLeftTop.y - Globals::glassSize.y);

    //if (!(verts[0].x > glassLeftTop.x &&
    //  verts[3].x < glassRightBottom.x &&
    //  verts[0].y < glassLeftTop.y &&
    //  verts[3].y > glassRightBottom.y))
    {
      const float xReps = 80.0f;
      const float yReps = xReps / Globals::gameBkSize.x * Globals::gameBkSize.y * 0.85f;
      const float texScaleCorrection = 0.56f; // tile deformation to get power of two texture size
      const float xVal = xReps * texScaleCorrection;
      const float yVal = yReps;

      glm::vec2 uv[4] =
      {
        { xVal * x / xfr, yVal * y / yfr },
        { xVal * x / xfr, yVal * (y + 1) / yfr },
        { xVal * (x + 1) / xfr, yVal * y / yfr },
        { xVal * (x + 1) / xfr, yVal * (y + 1) / yfr },
      };

      float fx0 = float(abs(xfr - 2 * x)) / xfr;
      float fy0 = float(y) / yfr;
      float fx1 = float(abs(xfr - 2 * (x + 1))) / xfr;
      float fy1 = float(y + 1) / yfr;

      float light[4] =
      {
        1.0f - sqrtf(fx0 * fx0 + fy0 * fy0) / M_SQRT2,
        1.0f - sqrtf(fx0 * fx0 + fy1 * fy1) / M_SQRT2,
        1.0f - sqrtf(fx1 * fx1 + fy0 * fy0) / M_SQRT2,
        1.0f - sqrtf(fx1 * fx1 + fy1 * fy1) / M_SQRT2,
      };

      glm::vec3 darkColor(0.05f, 0.1f, 0.2f);
      glm::vec3 lightColor(0.3f, 0.6f, 1.0f);

      glm::vec3 col[4] =
      {
        darkColor + (lightColor - darkColor) * light[0],
        darkColor + (lightColor - darkColor) * light[1],
        darkColor + (lightColor - darkColor) * light[2],
        darkColor + (lightColor - darkColor) * light[3],
      };

      addVertex(verts[0], uv[0], Globals::backgroundTexIndex, col[0], 1.0f);
      addVertex(verts[1], uv[1], Globals::backgroundTexIndex, col[1], 1.0f);
      addVertex(verts[2], uv[2], Globals::backgroundTexIndex, col[2], 1.0f);
      addVertex(verts[1], uv[1], Globals::backgroundTexIndex, col[1], 1.0f);
      addVertex(verts[2], uv[2], Globals::backgroundTexIndex, col[2], 1.0f);
      addVertex(verts[3], uv[3], Globals::backgroundTexIndex, col[3], 1.0f);
    }
  }
}

void MainMesh::buildGlassBackgroundMesh()
{
  const int xfr = 6;
  const int yfr = 6;

  for (int y = 0; y < yfr; y++)
  for (int x = 0; x < xfr; x++)
  {
    glm::vec2 verts[4] =
    {
      { Globals::glassSize.x * x / xfr, -Globals::glassSize.y * y / yfr },
      { Globals::glassSize.x * x / xfr, -Globals::glassSize.y * (y + 1) / yfr },
      { Globals::glassSize.x * (x + 1) / xfr, -Globals::glassSize.y * y / yfr },
      { Globals::glassSize.x * (x + 1) / xfr, -Globals::glassSize.y * (y + 1) / yfr },
    };

    float fx0 = float(abs(xfr - 2 * x)) / xfr;
    float fy0 = float(abs(xfr / 4 - y)) / yfr;
    float fx1 = float(abs(xfr - 2 * (x + 1))) / xfr;
    float fy1 = float(abs(xfr / 4 - (y + 1))) / yfr;

    float light[4] =
    {
      sqrtf(fx0 * fx0 + fy0 * fy0) / M_SQRT2,
      sqrtf(fx0 * fx0 + fy1 * fy1) / M_SQRT2,
      sqrtf(fx1 * fx1 + fy0 * fy0) / M_SQRT2,
      sqrtf(fx1 * fx1 + fy1 * fy1) / M_SQRT2,
    };

    glm::vec3 darkColor(0.1f, 0.25f, 0.45f);
    glm::vec3 lightColor(0.33f, 0.5f, 0.55f);

    glm::vec3 col[4] =
    {
      darkColor + (lightColor - darkColor) * light[0],
      darkColor + (lightColor - darkColor) * light[1],
      darkColor + (lightColor - darkColor) * light[2],
      darkColor + (lightColor - darkColor) * light[3],
    };

    glm::vec2 uv(0.5f, 0.5f);

    addVertex(verts[0], uv, Globals::emptyTexIndex, col[0], 1.0f);
    addVertex(verts[1], uv, Globals::emptyTexIndex, col[1], 1.0f);
    addVertex(verts[2], uv, Globals::emptyTexIndex, col[2], 1.0f);
    addVertex(verts[1], uv, Globals::emptyTexIndex, col[1], 1.0f);
    addVertex(verts[2], uv, Globals::emptyTexIndex, col[2], 1.0f);
    addVertex(verts[3], uv, Globals::emptyTexIndex, col[3], 1.0f);
  }
}

void MainMesh::buidGlassShadowMesh()
{
  const float pixSize = Globals::mainArrayTexturePixelSize;
  const float shadowWidth = 0.15f;
  const glm::vec3 zeroCol(0.0f, 0.0f, 0.0f);

  for (int y = 0; y < model.glassHeight; y++)
  for (int x = 0; x < model.glassWidth; x++)
  {
    Cell * cell = getGlassCell(x, y);

    if (cell->figureId)
    {
      Cell * rightCell = getGlassCell(x + 1, y);
      Cell * rightBottomCell = getGlassCell(x + 1, y + 1);
      Cell * bottomCell = getGlassCell(x, y + 1);

      if (bottomCell && bottomCell->figureId != cell->figureId)
      {
        Cell * leftCell = getGlassCell(x - 1, y);
        Cell * bottomLeftCell = getGlassCell(x - 1, y + 1);
        bool softLeft = !leftCell || leftCell->figureId != cell->figureId;

        glm::vec2 verts[4] =
        {
          { x, -y - 1.0f + pixSize },
          { x, -y - 1.0f - shadowWidth },
          { x + 1.0f, -y - 1.0f + pixSize },
          { x + 1.0f, -y - 1.0f - shadowWidth }
        };

        glm::vec2 uv[4] =
        {
          { softLeft ? 1.0f - pixSize: 0.5f, 0.5f },
          { 1.0f - pixSize,                  1.0f - pixSize },
          { 0.5f,                            0.5f },
          { 1.0f - pixSize,                  1.0f - pixSize }
        };

        if (softLeft)
          verts[1].x += shadowWidth;

        if (bottomLeftCell && bottomLeftCell->figureId == cell->figureId)
        {
          verts[0].x -= pixSize;
          verts[1].x += shadowWidth;
        }

        if (rightBottomCell && rightBottomCell->figureId != cell->figureId && rightCell && rightCell->figureId != cell->figureId)
        {
          verts[2].x -= pixSize;
          verts[3].x += shadowWidth;
        }

        addVertex(verts[0], uv[0], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(verts[1], uv[1], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(verts[2], uv[2], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(verts[1], uv[1], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(verts[2], uv[2], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(verts[3], uv[3], Globals::shadowTexIndex, zeroCol, 1.0f);
      }

      if (rightCell && rightCell->figureId != cell->figureId)
      {
        Cell * topCell = getGlassCell(x, y - 1);
        Cell * topRightCell = getGlassCell(x + 1, y - 1);
        bool softTop = !topCell || topCell->figureId != cell->figureId;

        glm::vec2 verts[4] =
        {
          { x + 1.0f - pixSize, -y },
          { x + 1.0f + shadowWidth, -y },
          { x + 1.0f - pixSize, -y - 1.0f },
          { x + 1.0f + shadowWidth, -y - 1.0f }
        };

        glm::vec2 uv[4] =
        {
          { softTop ? 1.0f - pixSize: 0.5f, 0.5f },
          { 1.0f - pixSize,                 1.0f - pixSize },
          { 0.5f,                           0.5f },
          { 1.0f - pixSize,                 1.0f - pixSize }
        };

        if (softTop)
          verts[1].y -= shadowWidth;

        if (topRightCell && topRightCell->figureId == cell->figureId)
        {
          verts[0].y += pixSize;
          verts[1].y -= shadowWidth;
        }

        if (rightBottomCell && rightBottomCell->figureId != cell->figureId && bottomCell && bottomCell->figureId != cell->figureId)
        {
          verts[2].y += pixSize;
          verts[3].y -= shadowWidth;
        }

        addVertex(verts[0], uv[0], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(verts[1], uv[1], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(verts[2], uv[2], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(verts[1], uv[1], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(verts[2], uv[2], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(verts[3], uv[3], Globals::shadowTexIndex, zeroCol, 1.0f);
      }
    }
  }
}

void MainMesh::buidGlassBlocksMesh()
{
  const float halfPixSize = Globals::mainArrayTexturePixelSize;

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

        const glm::vec2 segmentUvArray[3][3] =
        {
          { { 0.5f, 0.5f }, { 0.5f, 1.0f - halfPixSize }, { 0.0f + halfPixSize, 1.0f - halfPixSize }, }, // openSegment
          { { 0.5f, 0.5f }, { 0.0f + halfPixSize, 0.5f }, { 0.0f + halfPixSize, 0.0f + halfPixSize }, }, // partialSegment
          { { 0.5f, 0.5f }, { 0.5f, 0.0f + halfPixSize }, { 0.0f + halfPixSize, 0.0f + halfPixSize }, }, // borderedSegment
        };

        enum SegmentTypes { openSegment, partialSegment, borderedSegment };

        SegmentTypes horzSegmentType, vertSegmentType;

        if (haveHorzAdjCell && haveVertAdjCell && haveCornerAdjCell)
        {
          horzSegmentType = SegmentTypes::openSegment;
          vertSegmentType = SegmentTypes::openSegment;
        }
        else
        {
          if (haveHorzAdjCell)
            horzSegmentType = SegmentTypes::partialSegment;
          else
            horzSegmentType = SegmentTypes::borderedSegment;

          if (haveVertAdjCell)
            vertSegmentType = SegmentTypes::partialSegment;
          else
            vertSegmentType = SegmentTypes::borderedSegment;
        }

        const float dx = float(i & 1);
        const float dy = float((i & 2) >> 1);

        glm::vec2 verts[4] =
        {
          { x + 0.5f, -y - 0.5f },
          { x + 0.5f, -y - dy },
          { x + dx, -y - dy },
          { x + dx, -y - 0.5f },
        };

        addVertex(verts[0], segmentUvArray[vertSegmentType][0], Globals::blockTemplateTexIndex, blockColors[cell->color], 1.0f);
        addVertex(verts[1], segmentUvArray[vertSegmentType][1], Globals::blockTemplateTexIndex, blockColors[cell->color], 1.0f);
        addVertex(verts[2], segmentUvArray[vertSegmentType][2], Globals::blockTemplateTexIndex, blockColors[cell->color], 1.0f);
        addVertex(verts[0], segmentUvArray[horzSegmentType][0], Globals::blockTemplateTexIndex, blockColors[cell->color], 1.0f);
        addVertex(verts[3], segmentUvArray[horzSegmentType][1], Globals::blockTemplateTexIndex, blockColors[cell->color], 1.0f);
        addVertex(verts[2], segmentUvArray[horzSegmentType][2], Globals::blockTemplateTexIndex, blockColors[cell->color], 1.0f);
      }
    }
  }
}

void MainMesh::biuldGlassGlowMesh()
{
  const float pixSize = Globals::mainArrayTexturePixelSize;
  const float glowWidth = 0.5f;
  const float glowMinAlpha = 0.01f;
  const float glowMaxAlpha = 0.25f;

  for (int y = 0; y < model.glassHeight; y++)
  for (int x = 0; x < model.glassWidth; x++)
  {
    Cell * cell = getGlassCell(x, y);

    if (cell->figureId)
    {
      Cell * leftCell = getGlassCell(x - 1, y);
      Cell * leftTopCell = getGlassCell(x - 1, y - 1);
      Cell * topCell = getGlassCell(x, y - 1);
      Cell * topRightCell = getGlassCell(x + 1, y - 1);
      Cell * rightCell = getGlassCell(x + 1, y);
      Cell * rightBottomCell = getGlassCell(x + 1, y + 1);
      Cell * bottomCell = getGlassCell(x, y + 1);
      Cell * bottomLeftCell = getGlassCell(x - 1, y + 1);

      bool haveLeftCell = leftCell && leftCell->figureId == cell->figureId;
      bool haveLeftTopCell = leftTopCell && leftTopCell->figureId == cell->figureId;
      bool haveTopCell = topCell && topCell->figureId == cell->figureId;
      bool haveTopRightCell = topRightCell && topRightCell->figureId == cell->figureId;
      bool haveRightCell = rightCell && rightCell->figureId == cell->figureId;
      bool haveRightBottomCell = rightBottomCell && rightBottomCell->figureId == cell->figureId;
      bool haveBottomCell = bottomCell && bottomCell->figureId == cell->figureId;
      bool haveBottomLeftCell = bottomLeftCell && bottomLeftCell->figureId == cell->figureId;

      if (leftCell && leftCell->figureId != cell->figureId)
      {
        glm::vec2 verts[4] =
        {
          { x + pixSize, -y },
          { x - glowWidth, -y },
          { x + pixSize, -y - 1.0f },
          { x - glowWidth, -y - 1.0f }
        };

        if (haveLeftTopCell)
        {
          verts[0].y += pixSize;
          verts[1].y -= glowWidth;
        }
        else if (topCell && !haveTopCell)
        {
          verts[0].y -= pixSize;
          verts[1].y += glowWidth;
        }

        if (haveBottomLeftCell)
        {
          verts[2].y -= pixSize;
          verts[3].y += glowWidth;
        }
        else if (bottomCell && !haveBottomCell)
        {
          verts[2].y += pixSize;
          verts[3].y -= glowWidth;
        }

        addVertex(verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMaxAlpha, 0.0f);
        addVertex(verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMinAlpha, 0.0f);
        addVertex(verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMaxAlpha, 0.0f);
        addVertex(verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMinAlpha, 0.0f);
        addVertex(verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMaxAlpha, 0.0f);
        addVertex(verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMinAlpha, 0.0f);
      }

      if (rightCell && rightCell->figureId != cell->figureId)
      {
        glm::vec2 verts[4] =
        {
          { x + 1.0f - pixSize, -y },
          { x + 1.0f + glowWidth, -y },
          { x + 1.0f - pixSize, -y - 1.0f },
          { x + 1.0f + glowWidth, -y - 1.0f }
        };

        if (haveTopRightCell)
        {
          verts[0].y += pixSize;
          verts[1].y -= glowWidth;
        }
        else if (topCell && !haveTopCell)
        {
          verts[0].y -= pixSize;
          verts[1].y += glowWidth;
        }

        if (haveRightBottomCell)
        {
          verts[2].y -= pixSize;
          verts[3].y += glowWidth;
        }
        else if (bottomCell && !haveBottomCell)
        {
          verts[2].y += pixSize;
          verts[3].y -= glowWidth;
        }

        addVertex(verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMaxAlpha, 0.0f);
        addVertex(verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMinAlpha, 0.0f);
        addVertex(verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMaxAlpha, 0.0f);
        addVertex(verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMinAlpha, 0.0f);
        addVertex(verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMaxAlpha, 0.0f);
        addVertex(verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMinAlpha, 0.0f);
      }

      if (topCell && topCell->figureId != cell->figureId)
      {
        glm::vec2 verts[4] =
        {
          { x, -y - pixSize },
          { x, -y + glowWidth },
          { x + 1.0f, -y - pixSize },
          { x + 1.0f, -y + glowWidth }
        };

        if (haveLeftTopCell)
        {
          verts[0].x -= pixSize;
          verts[1].x += glowWidth;
        }
        else if (leftCell && !haveLeftCell)
        {
          verts[0].x += pixSize;
          verts[1].x -= glowWidth;
        }

        if (haveTopRightCell)
        {
          verts[2].x += pixSize;
          verts[3].x -= glowWidth;
        }
        else if (rightCell && !haveRightCell)
        {
          verts[2].x -= pixSize;
          verts[3].x += glowWidth;
        }

        addVertex(verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMaxAlpha, 0.0f);
        addVertex(verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMinAlpha, 0.0f);
        addVertex(verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMaxAlpha, 0.0f);
        addVertex(verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMinAlpha, 0.0f);
        addVertex(verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMaxAlpha, 0.0f);
        addVertex(verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMinAlpha, 0.0f);
      }

      if (bottomCell && bottomCell->figureId != cell->figureId)
      {
        glm::vec2 verts[4] =
        {
          { x, -y - 1.0f + pixSize },
          { x, -y - 1.0f - glowWidth },
          { x + 1.0f, -y - 1.0f + pixSize },
          { x + 1.0f, -y - 1.0f - glowWidth }
        };

        if (haveBottomLeftCell)
        {
          verts[0].x -= pixSize;
          verts[1].x += glowWidth;
        }
        else if (leftCell && !haveLeftCell)
        {
          verts[0].x += pixSize;
          verts[1].x -= glowWidth;
        }

        if (haveRightBottomCell)
        {
          verts[2].x += pixSize;
          verts[3].x -= glowWidth;
        }
        else if (rightCell && !haveRightCell)
        {
          verts[2].x -= pixSize;
          verts[3].x += glowWidth;
        }

        addVertex(verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMaxAlpha, 0.0f);
        addVertex(verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMinAlpha, 0.0f);
        addVertex(verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMaxAlpha, 0.0f);
        addVertex(verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMinAlpha, 0.0f);
        addVertex(verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMaxAlpha, 0.0f);
        addVertex(verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, blockColors[cell->color] * glowMinAlpha, 0.0f);
      }
    }
  }
}
