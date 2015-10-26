#define _USE_MATH_DEFINES
#include <math.h>
#include "MainMesh.h"
#include "Globals.h"
#include "DropSparkle.h"
#include "DropTrail.h"

MainMesh::MainMesh(Model & model) :
  figureVert(GL_VERTEX_SHADER),
  figureFrag(GL_FRAGMENT_SHADER),
  model(model)
{
}

MainMesh::~MainMesh()
{
}

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
    "  float alpha = (1.0f - texcol.b) * color.a;"
    "  vec3 rgb = mix(texcol.r * color.rgb, texcol.rrr, texcol.g);"
    "  out_color = vec4(rgb + (2.0f - rand(pixPos)) / 150.0f * alpha, alpha);"
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

  buildBackgroundMesh();
  buidGlassShadowMesh();
  buidGlassBlocksMesh();
  biuldGlassGlowMesh();
  buildFigureBlocksMesh();
  buildFigureGlowMesh();
  buildDropTrailsMesh();
  buildRowFlashesMesh();

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

  if (!cell->figureId && !model.haveFallingRows)
  {
    bool isInCurFigure =
      x >= model.curFigureX &&
      x < model.curFigureX + model.curFigure.dim &&
      y >= model.curFigureY &&
      y < model.curFigureY + model.curFigure.dim;

    if (isInCurFigure)
      cell = model.curFigure.cells.data() + x - model.curFigureX + (y - model.curFigureY) * model.curFigure.dim;
  }

  return cell;
}

Cell * MainMesh::getFigureCell(Figure & figure, int x, int y)
{
  if (x < 0 || y < 0 || x >= figure.dim || y >= figure.dim)
    return NULL;

  Cell * cell = figure.cells.data() + x + y * figure.dim;
  return cell;
}

void MainMesh::addVertex(const glm::vec2 & xy, const glm::vec2 & uv, int texIndex, const glm::vec3 & color, float alpha)
{
  vertexBuffer.push_back(xy.x);
  vertexBuffer.push_back(xy.y);
  vertexBuffer.push_back(uv.x);
  vertexBuffer.push_back(uv.y);
  vertexBuffer.push_back(float(texIndex));
  vertexBuffer.push_back(color.r);
  vertexBuffer.push_back(color.g);
  vertexBuffer.push_back(color.b);
  vertexBuffer.push_back(alpha);
  vertexCount++;
}

void MainMesh::buildBackgroundMesh()
{
  // add game base background to mesh

  glm::vec2 origin = Globals::gameBkPos;

  for (int y = 0, height = 6; y < height; y++)
  for (int x = 0, width = 6; x < width; x++)
  {
    glm::vec2 verts[4] =
    {
      { Globals::gameBkSize.x * x / width,       -Globals::gameBkSize.y * y / height },
      { Globals::gameBkSize.x * x / width,       -Globals::gameBkSize.y * (y + 1) / height },
      { Globals::gameBkSize.x * (x + 1) / width, -Globals::gameBkSize.y * y / height },
      { Globals::gameBkSize.x * (x + 1) / width, -Globals::gameBkSize.y * (y + 1) / height },
    };

    const float xReps = 80.0f;
    const float yReps = xReps / Globals::gameBkSize.x * Globals::gameBkSize.y * 0.85f;
    const float texScaleCorrection = 0.56f; // tile deformation to get power of two texture size
    const float xVal = xReps * texScaleCorrection;
    const float yVal = yReps;

    glm::vec2 uv[4] =
    {
      { xVal * x / width, yVal * y / height },
      { xVal * x / width, yVal * (y + 1) / height },
      { xVal * (x + 1) / width, yVal * y / height },
      { xVal * (x + 1) / width, yVal * (y + 1) / height },
    };

    float fx0 = float(abs(width - 2 * x)) / width;
    float fy0 = float(y) / height;
    float fx1 = float(abs(width - 2 * (x + 1))) / width;
    float fy1 = float(y + 1) / height;

    float light[4] =
    {
      1.0f - sqrtf(fx0 * fx0 + fy0 * fy0) / float(M_SQRT2),
      1.0f - sqrtf(fx0 * fx0 + fy1 * fy1) / float(M_SQRT2),
      1.0f - sqrtf(fx1 * fx1 + fy0 * fy0) / float(M_SQRT2),
      1.0f - sqrtf(fx1 * fx1 + fy1 * fy1) / float(M_SQRT2),
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

    addVertex(origin + verts[0], uv[0], Globals::backgroundTexIndex, col[0], 1.0f);
    addVertex(origin + verts[1], uv[1], Globals::backgroundTexIndex, col[1], 1.0f);
    addVertex(origin + verts[2], uv[2], Globals::backgroundTexIndex, col[2], 1.0f);
    addVertex(origin + verts[1], uv[1], Globals::backgroundTexIndex, col[1], 1.0f);
    addVertex(origin + verts[2], uv[2], Globals::backgroundTexIndex, col[2], 1.0f);
    addVertex(origin + verts[3], uv[3], Globals::backgroundTexIndex, col[3], 1.0f);
  }

  // add glass background to mesh

  origin = Globals::glassPos;

  for (int y = 0, height = 6; y < height; y++)
  for (int x = 0, width = 6; x < width; x++)
  {
    glm::vec2 verts[4] =
    {
      { Globals::glassSize.x * x / width, -Globals::glassSize.y * y / height },
      { Globals::glassSize.x * x / width, -Globals::glassSize.y * (y + 1) / height },
      { Globals::glassSize.x * (x + 1) / width, -Globals::glassSize.y * y / height },
      { Globals::glassSize.x * (x + 1) / width, -Globals::glassSize.y * (y + 1) / height },
    };

    float fx0 = float(abs(width - 2 * x)) / width;
    float fy0 = float(abs(width / 4 - y)) / height;
    float fx1 = float(abs(width - 2 * (x + 1))) / width;
    float fy1 = float(abs(width / 4 - (y + 1))) / height;

    float light[4] =
    {
      sqrtf(fx0 * fx0 + fy0 * fy0) / float(M_SQRT2),
      sqrtf(fx0 * fx0 + fy1 * fy1) / float(M_SQRT2),
      sqrtf(fx1 * fx1 + fy0 * fy0) / float(M_SQRT2),
      sqrtf(fx1 * fx1 + fy1 * fy1) / float(M_SQRT2),
    };

    double a = 0.0f;
    static double freq = (double)Crosy::getPerformanceFrequency();
    if (freq)
      a = double(Crosy::getPerformanceCounter()) / freq / 300.0f;

    const float rMin = 0.1f;
    const float gMin = 0.1f;
    const float bMin = 0.2f;
    const float rMax = 0.3f;
    const float gMax = 0.3f;
    const float bMax = 0.5f;
    const float darkMul = 0.25f;
    const float darkDiff = (float)M_PI / 20.0f;
    const float gDiff = (float)M_PI / 3.0f;
    const float bDiff = 2.0f * (float)M_PI / 3.0f;

    glm::vec3 lightColor(rMin + (rMax - rMin) * abs(sin(a)),
                         gMin + (gMax - gMin) * abs(sin(a + gDiff)),
                         bMin + (bMax - bMin) * abs(sin(a + bDiff)));

    glm::vec3 darkColor(darkMul * (rMin + (rMax - rMin) * abs(sin(a + darkDiff))),
                        darkMul * (gMin + (gMax - gMin) * abs(sin(a + gDiff + darkDiff))),
                        darkMul * (bMin + (bMax - bMin) * abs(sin(a + bDiff + darkDiff))));

    glm::vec3 col[4] =
    {
      darkColor + (lightColor - darkColor) * light[0],
      darkColor + (lightColor - darkColor) * light[1],
      darkColor + (lightColor - darkColor) * light[2],
      darkColor + (lightColor - darkColor) * light[3],
    };

    glm::vec2 uv(0.5f, 0.5f);

    addVertex(origin + verts[0], uv, Globals::emptyTexIndex, col[0], 1.0f);
    addVertex(origin + verts[1], uv, Globals::emptyTexIndex, col[1], 1.0f);
    addVertex(origin + verts[2], uv, Globals::emptyTexIndex, col[2], 1.0f);
    addVertex(origin + verts[1], uv, Globals::emptyTexIndex, col[1], 1.0f);
    addVertex(origin + verts[2], uv, Globals::emptyTexIndex, col[2], 1.0f);
    addVertex(origin + verts[3], uv, Globals::emptyTexIndex, col[3], 1.0f);
  }

  // add hold figure and next figure backgrounds to mesh

  const float pixSize = Globals::mainArrayTexturePixelSize;

  origin =
  { 
    Globals::glassPos.x - Globals::holdNextBkHorzGap - Globals::holdNextBkSize,
    Globals::glassPos.y - Globals::holdNextTitleHeight 
  };
  
  glm::vec2 verts[4] =
  {
    { 0.0f,                     0.0f },
    { 0.0f,                     -Globals::holdNextBkSize },
    { Globals::holdNextBkSize,  0.0f },
    { Globals::holdNextBkSize,  -Globals::holdNextBkSize },
  };

  glm::vec2 uv[4] =
  {
    { pixSize,        pixSize },
    { pixSize,        1.0f - pixSize },
    { 1.0f - pixSize, pixSize },
    { 1.0f - pixSize, 1.0f - pixSize }
  };

  glm::vec3 color(0.5f);

  if (model.holdFigure.color != Globals::Color::clNone)
    color = Globals::ColorValues[model.holdFigure.color];

  addVertex(origin + verts[0], uv[0], Globals::holdFigureBkTexIndex, color, 1.0f);
  addVertex(origin + verts[1], uv[1], Globals::holdFigureBkTexIndex, color, 1.0f);
  addVertex(origin + verts[2], uv[2], Globals::holdFigureBkTexIndex, color, 1.0f);
  addVertex(origin + verts[1], uv[1], Globals::holdFigureBkTexIndex, color, 1.0f);
  addVertex(origin + verts[2], uv[2], Globals::holdFigureBkTexIndex, color, 1.0f);
  addVertex(origin + verts[3], uv[3], Globals::holdFigureBkTexIndex, color, 1.0f);

  origin.x += Globals::holdNextBkSize + Globals::glassSize.x + 2.0f * Globals::holdNextBkHorzGap;

  if (model.nextFigures[0].color != Globals::Color::clNone)
    color = Globals::ColorValues[model.nextFigures[0].color];

  addVertex(origin + verts[0], uv[0], Globals::nextFigureBkTexIndex, color, 1.0f);
  addVertex(origin + verts[1], uv[1], Globals::nextFigureBkTexIndex, color, 1.0f);
  addVertex(origin + verts[2], uv[2], Globals::nextFigureBkTexIndex, color, 1.0f);
  addVertex(origin + verts[1], uv[1], Globals::nextFigureBkTexIndex, color, 1.0f);
  addVertex(origin + verts[2], uv[2], Globals::nextFigureBkTexIndex, color, 1.0f);
  addVertex(origin + verts[3], uv[3], Globals::nextFigureBkTexIndex, color, 1.0f);


}

void MainMesh::buidGlassShadowMesh()
{
  const float pixSize = Globals::mainArrayTexturePixelSize;
  const float shadowWidth = 0.15f;
  const glm::vec3 zeroCol(0.0f, 0.0f, 0.0f);
  const float scale = Globals::glassSize.x / model.glassWidth;

  for (int y = 0; y < model.glassHeight; y++)
  for (int x = 0; x < model.glassWidth; x++)
  {
    Cell * cell = getGlassCell(x, y);
    glm::vec2 origin = Globals::glassPos;
    
    if (model.rowElevation[y])
      origin.y += scale * model.rowCurrentElevation[y];

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
          { x,        -y - 1.0f + pixSize },
          { x,        -y - 1.0f - shadowWidth },
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

        addVertex(origin + scale * verts[0], uv[0], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[1], uv[1], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[2], uv[2], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[1], uv[1], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[2], uv[2], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[3], uv[3], Globals::shadowTexIndex, zeroCol, 1.0f);
      }

      if (rightCell && rightCell->figureId != cell->figureId)
      {
        Cell * topCell = getGlassCell(x, y - 1);
        Cell * topRightCell = getGlassCell(x + 1, y - 1);
        bool softTop = !topCell || topCell->figureId != cell->figureId;

        glm::vec2 verts[4] =
        {
          { x + 1.0f - pixSize,     -y },
          { x + 1.0f + shadowWidth, -y },
          { x + 1.0f - pixSize,     -y - 1.0f },
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

        addVertex(origin + scale * verts[0], uv[0], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[1], uv[1], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[2], uv[2], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[1], uv[1], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[2], uv[2], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[3], uv[3], Globals::shadowTexIndex, zeroCol, 1.0f);
      }
    }
  }
}

void MainMesh::buidGlassBlocksMesh()
{
  const float pixSize = Globals::mainArrayTexturePixelSize;
  const float scale = Globals::glassSize.x / model.glassWidth;

  for (int y = 0; y < model.glassHeight; y++)
  for (int x = 0; x < model.glassWidth; x++)
  {
    Cell * cell = getGlassCell(x, y);
    glm::vec2 origin = Globals::glassPos;
    
    if (model.rowElevation[y])
      origin.y += scale * model.rowCurrentElevation[y];

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
          { { 0.5f, 0.5f }, { 0.5f, 1.0f - pixSize }, { 0.0f + pixSize, 1.0f - pixSize }, }, // openSegment
          { { 0.5f, 0.5f }, { 0.0f + pixSize, 0.5f }, { 0.0f + pixSize, 0.0f + pixSize }, }, // partialSegment
          { { 0.5f, 0.5f }, { 0.5f, 0.0f + pixSize }, { 0.0f + pixSize, 0.0f + pixSize }, }, // borderedSegment
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

        const glm::vec3 color = Globals::ColorValues[cell->color];

        addVertex(origin + scale * verts[0], segmentUvArray[vertSegmentType][0], Globals::blockTemplateTexIndex, color, 1.0f);
        addVertex(origin + scale * verts[1], segmentUvArray[vertSegmentType][1], Globals::blockTemplateTexIndex, color, 1.0f);
        addVertex(origin + scale * verts[2], segmentUvArray[vertSegmentType][2], Globals::blockTemplateTexIndex, color, 1.0f);
        addVertex(origin + scale * verts[0], segmentUvArray[horzSegmentType][0], Globals::blockTemplateTexIndex, color, 1.0f);
        addVertex(origin + scale * verts[3], segmentUvArray[horzSegmentType][1], Globals::blockTemplateTexIndex, color, 1.0f);
        addVertex(origin + scale * verts[2], segmentUvArray[horzSegmentType][2], Globals::blockTemplateTexIndex, color, 1.0f);
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
  const float scale = Globals::glassSize.x / model.glassWidth;

  for (int y = 0; y < model.glassHeight; y++)
  for (int x = 0; x < model.glassWidth; x++)
  {
    Cell * cell = getGlassCell(x, y);
    glm::vec2 origin = Globals::glassPos;

    if (model.rowElevation[y])
      origin.y += scale * model.rowCurrentElevation[y];

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

      const glm::vec3 color = Globals::ColorValues[cell->color];

      if (leftCell && leftCell->figureId != cell->figureId)
      {
        glm::vec2 verts[4] =
        {
          { x + pixSize,   -y },
          { x - glowWidth, -y },
          { x + pixSize,   -y - 1.0f },
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

        addVertex(origin + scale * verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
        addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
        addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
      }

      if (rightCell && rightCell->figureId != cell->figureId)
      {
        glm::vec2 verts[4] =
        {
          { x + 1.0f - pixSize,   -y },
          { x + 1.0f + glowWidth, -y },
          { x + 1.0f - pixSize,   -y - 1.0f },
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

        addVertex(origin + scale * verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
        addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
        addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
      }

      if (topCell && topCell->figureId != cell->figureId)
      {
        glm::vec2 verts[4] =
        {
          { x,        -y - pixSize },
          { x,        -y + glowWidth },
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

        addVertex(origin + scale * verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
        addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
        addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
      }

      if (bottomCell && bottomCell->figureId != cell->figureId)
      {
        glm::vec2 verts[4] =
        {
          { x,        -y - 1.0f + pixSize },
          { x,        -y - 1.0f - glowWidth },
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

        addVertex(origin + scale * verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
        addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
        addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
      }
    }
  }
}

void MainMesh::buildFigureBlocksMesh()
{
  const float pixSize = Globals::mainArrayTexturePixelSize;
  const float scale = Globals::holdNextBkSize * 0.75f / 4.0f;

  for (int i = -1; i < Globals::nextFiguresCount; i++)
  {

    Figure * figure = NULL;

    if (i < 0)
      figure = &model.holdFigure;
    else
      figure = &model.nextFigures[i];

    if (figure->dim)
    {

      glm::vec2 origin;

      int figureLeftGap = figure->dim;
      int figureWidth = 0;
      int figureTopGap = figure->dim;
      int figureHeight = 0;


      for (int x = 0; x < figure->dim; x++)
      {
        bool horzEmpty = true;
        bool vertEmpty = true;

        for (int y = 0; y < figure->dim; y++)
        {
          if (!figure->cells[y + x * figure->dim].isEmpty())
            horzEmpty = false;

          if (!figure->cells[x + y * figure->dim].isEmpty())
            vertEmpty = false;
        }

        if (!horzEmpty)
        {
          if(x < figureTopGap)
            figureTopGap = x;

          if (x > figureHeight)
            figureHeight = x;
        }

        if (!vertEmpty)
        {
          if(x < figureLeftGap)
            figureLeftGap = x;

          if (x > figureWidth)
            figureWidth = x;
        }
      }

      figureHeight -= figureTopGap - 1;
      figureWidth -= figureLeftGap - 1;

      if (i < 0)
      {
        origin.x = Globals::glassPos.x - Globals::holdNextBkHorzGap - 0.5f * Globals::holdNextBkSize - scale * 0.5f * figureWidth - scale * figureLeftGap;
        origin.y = Globals::glassPos.y - Globals::holdNextTitleHeight - 0.5f * Globals::holdNextBkSize + 0.5f * scale * figureHeight + scale * figureTopGap;
      }
      else
      {
        origin.x = Globals::glassPos.x + Globals::glassSize.x + Globals::holdNextBkHorzGap + 0.5f * Globals::holdNextBkSize - scale * 0.5f * float(figureWidth) - scale * float(figureLeftGap);
        origin.y = Globals::glassPos.y - Globals::holdNextTitleHeight - 0.5f * Globals::holdNextBkSize + scale * 0.5f * figureHeight + scale * figureTopGap - i * Globals::holdNextBkSize;
      }

      for (int y = 0; y < figure->dim; y++)
      for (int x = 0; x < figure->dim; x++)
      {
        Cell * cell = getFigureCell(*figure, x, y);

        if (cell && !cell->isEmpty())
        {
          for (int i = 0; i < 4; i++)
          {
            int cornerDX = (i & 1) * 2 - 1;
            int cornerDY = (i & 2) - 1;

            Cell * horzAdjCell = getFigureCell(*figure, x + cornerDX, y);
            Cell * vertAdjCell = getFigureCell(*figure, x, y + cornerDY);
            Cell * cornerAdjCell = getFigureCell(*figure, x + cornerDX, y + cornerDY);

            bool haveHorzAdjCell = (horzAdjCell && !horzAdjCell->isEmpty());
            bool haveVertAdjCell = (vertAdjCell && !vertAdjCell->isEmpty());
            bool haveCornerAdjCell = (cornerAdjCell && !cornerAdjCell->isEmpty());

            const glm::vec2 segmentUvArray[3][3] =
            {
              { { 0.5f, 0.5f }, { 0.5f, 1.0f - pixSize }, { 0.0f + pixSize, 1.0f - pixSize }, }, // openSegment
              { { 0.5f, 0.5f }, { 0.0f + pixSize, 0.5f }, { 0.0f + pixSize, 0.0f + pixSize }, }, // partialSegment
              { { 0.5f, 0.5f }, { 0.5f, 0.0f + pixSize }, { 0.0f + pixSize, 0.0f + pixSize }, }, // borderedSegment
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

            const glm::vec3 color = Globals::ColorValues[cell->color];

            addVertex(origin + scale * verts[0], segmentUvArray[vertSegmentType][0], Globals::boldBlockTemplateTexIndex, color, 1.0f);
            addVertex(origin + scale * verts[1], segmentUvArray[vertSegmentType][1], Globals::boldBlockTemplateTexIndex, color, 1.0f);
            addVertex(origin + scale * verts[2], segmentUvArray[vertSegmentType][2], Globals::boldBlockTemplateTexIndex, color, 1.0f);
            addVertex(origin + scale * verts[0], segmentUvArray[horzSegmentType][0], Globals::boldBlockTemplateTexIndex, color, 1.0f);
            addVertex(origin + scale * verts[3], segmentUvArray[horzSegmentType][1], Globals::boldBlockTemplateTexIndex, color, 1.0f);
            addVertex(origin + scale * verts[2], segmentUvArray[horzSegmentType][2], Globals::boldBlockTemplateTexIndex, color, 1.0f);
          }
        }
      }
    }
  }
}

void MainMesh::buildFigureGlowMesh()
{
  const float pixSize = Globals::mainArrayTexturePixelSize;
  const float glowWidth = 0.3f;
  const float glowMinAlpha = 0.01f;
  const float glowMaxAlpha = 0.25f;
  const float scale = Globals::holdNextBkSize * 0.75f / 4.0f;

  for (int i = -1; i < Globals::nextFiguresCount; i++)
  {

    Figure * figure = NULL;

    if (i < 0)
      figure = &model.holdFigure;
    else
      figure = &model.nextFigures[i];

    if (figure->dim)
    {

      glm::vec2 origin;

      int figureLeftGap = figure->dim;
      int figureWidth = 0;
      int figureTopGap = figure->dim;
      int figureHeight = 0;


      for (int x = 0; x < figure->dim; x++)
      {
        bool horzEmpty = true;
        bool vertEmpty = true;

        for (int y = 0; y < figure->dim; y++)
        {
          if (!figure->cells[y + x * figure->dim].isEmpty())
            horzEmpty = false;

          if (!figure->cells[x + y * figure->dim].isEmpty())
            vertEmpty = false;
        }

        if (!horzEmpty)
        {
          if (x < figureTopGap)
            figureTopGap = x;

          if (x > figureHeight)
            figureHeight = x;
        }

        if (!vertEmpty)
        {
          if (x < figureLeftGap)
            figureLeftGap = x;

          if (x > figureWidth)
            figureWidth = x;
        }
      }

      figureHeight -= figureTopGap - 1;
      figureWidth -= figureLeftGap - 1;

      if (i < 0)
      {
        origin.x = Globals::glassPos.x - Globals::holdNextBkHorzGap - 0.5f * Globals::holdNextBkSize - scale * 0.5f * figureWidth - scale * figureLeftGap;
        origin.y = Globals::glassPos.y - Globals::holdNextTitleHeight - 0.5f * Globals::holdNextBkSize + 0.5f * scale * figureHeight + scale * figureTopGap;
      }
      else
      {
        origin.x = Globals::glassPos.x + Globals::glassSize.x + Globals::holdNextBkHorzGap + 0.5f * Globals::holdNextBkSize - scale * 0.5f * float(figureWidth) - scale * float(figureLeftGap);
        origin.y = Globals::glassPos.y - Globals::holdNextTitleHeight - 0.5f * Globals::holdNextBkSize + scale * 0.5f * figureHeight + scale * figureTopGap - i * Globals::holdNextBkSize;
      }

      for (int y = 0; y < figure->dim; y++)
      for (int x = 0; x < figure->dim; x++)
      {
        Cell * cell = getFigureCell(*figure, x, y);

        if (cell && !cell->isEmpty())
        {
          Cell * leftCell = getFigureCell(*figure, x - 1, y);
          Cell * leftTopCell = getFigureCell(*figure, x - 1, y - 1);
          Cell * topCell = getFigureCell(*figure, x, y - 1);
          Cell * topRightCell = getFigureCell(*figure, x + 1, y - 1);
          Cell * rightCell = getFigureCell(*figure, x + 1, y);
          Cell * rightBottomCell = getFigureCell(*figure, x + 1, y + 1);
          Cell * bottomCell = getFigureCell(*figure, x, y + 1);
          Cell * bottomLeftCell = getFigureCell(*figure, x - 1, y + 1);

          bool haveLeftCell = leftCell && !leftCell->isEmpty();
          bool haveLeftTopCell = leftTopCell && !leftTopCell->isEmpty();
          bool haveTopCell = topCell && !topCell->isEmpty();
          bool haveTopRightCell = topRightCell && !topRightCell->isEmpty();
          bool haveRightCell = rightCell && !rightCell->isEmpty();
          bool haveRightBottomCell = rightBottomCell && !rightBottomCell->isEmpty();
          bool haveBottomCell = bottomCell && !bottomCell->isEmpty();
          bool haveBottomLeftCell = bottomLeftCell && !bottomLeftCell->isEmpty();

          const glm::vec3 color = Globals::ColorValues[cell->color];

          if (!haveLeftCell)
          {
            glm::vec2 verts[4] =
            {
              { x + pixSize,   -y },
              { x - glowWidth, -y },
              { x + pixSize,   -y - 1.0f },
              { x - glowWidth, -y - 1.0f }
            };

            if (haveLeftTopCell)
            {
              verts[0].y += pixSize;
              verts[1].y -= glowWidth;
            }
            else if (!haveTopCell)
            {
              verts[0].y -= pixSize;
              verts[1].y += glowWidth;
            }

            if (haveBottomLeftCell)
            {
              verts[2].y -= pixSize;
              verts[3].y += glowWidth;
            }
            else if (!haveBottomCell)
            {
              verts[2].y += pixSize;
              verts[3].y -= glowWidth;
            }

            addVertex(origin + scale * verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
            addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
            addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
          }

          if (!haveRightCell)
          {
            glm::vec2 verts[4] =
            {
              { x + 1.0f - pixSize,   -y },
              { x + 1.0f + glowWidth, -y },
              { x + 1.0f - pixSize,   -y - 1.0f },
              { x + 1.0f + glowWidth, -y - 1.0f }
            };

            if (haveTopRightCell)
            {
              verts[0].y += pixSize;
              verts[1].y -= glowWidth;
            }
            else if (!haveTopCell)
            {
              verts[0].y -= pixSize;
              verts[1].y += glowWidth;
            }

            if (haveRightBottomCell)
            {
              verts[2].y -= pixSize;
              verts[3].y += glowWidth;
            }
            else if (!haveBottomCell)
            {
              verts[2].y += pixSize;
              verts[3].y -= glowWidth;
            }

            addVertex(origin + scale * verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
            addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
            addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
          }

          if (!haveTopCell)
          {
            glm::vec2 verts[4] =
            {
              { x,        -y - pixSize },
              { x,        -y + glowWidth },
              { x + 1.0f, -y - pixSize },
              { x + 1.0f, -y + glowWidth }
            };

            if (haveLeftTopCell)
            {
              verts[0].x -= pixSize;
              verts[1].x += glowWidth;
            }
            else if (!haveLeftCell)
            {
              verts[0].x += pixSize;
              verts[1].x -= glowWidth;
            }

            if (haveTopRightCell)
            {
              verts[2].x += pixSize;
              verts[3].x -= glowWidth;
            }
            else if (!haveRightCell)
            {
              verts[2].x -= pixSize;
              verts[3].x += glowWidth;
            }

            addVertex(origin + scale * verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
            addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
            addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
          }

          if (!haveBottomCell)
          {
            glm::vec2 verts[4] =
            {
              { x,        -y - 1.0f + pixSize },
              { x,        -y - 1.0f - glowWidth },
              { x + 1.0f, -y - 1.0f + pixSize },
              { x + 1.0f, -y - 1.0f - glowWidth }
            };

            if (haveBottomLeftCell)
            {
              verts[0].x -= pixSize;
              verts[1].x += glowWidth;
            }
            else if (!haveLeftCell)
            {
              verts[0].x += pixSize;
              verts[1].x -= glowWidth;
            }

            if (haveRightBottomCell)
            {
              verts[2].x += pixSize;
              verts[3].x -= glowWidth;
            }
            else if (!haveRightCell)
            {
              verts[2].x -= pixSize;
              verts[3].x += glowWidth;
            }

            addVertex(origin + scale * verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
            addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
            addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
          }
        }
      }
    }
  }
}

void MainMesh::buildDropTrailsMesh()
{
  const glm::vec2 origin = Globals::glassPos;
  const float scale = Globals::glassSize.x / model.glassWidth;
  const float pixSize = Globals::mainArrayTexturePixelSize;

  for (std::list<DropTrail>::iterator it = model.dropTrails.begin(); it != model.dropTrails.end(); ++it)
  {
    float currentTrailAlpha = 1.0f - it->getTrailProgress();
    float currentTrailHeight = float(it->height) * currentTrailAlpha;
    float trailWidth = scale * 1.25f;
    const float dx = 0.22f;
    const float dy = 0.1f * currentTrailHeight;

    glm::vec2 verts[4] =
    {
      { glm::max<float>(it->x - dx, 0.0f),                                  -glm::max<float>(it->y - currentTrailHeight, 0.0f)  },
      { glm::min<float>(it->x + 1.0f + 2.0f * dx, (float)model.glassWidth), -glm::max<float>(it->y - currentTrailHeight, 0.0f) },
      { glm::max<float>(it->x - dx, 0.0f),                                  -glm::min<float>(it->y + dy, (float)model.glassHeight) },
      { glm::min<float>(it->x + 1.0f + 2.0f * dx, (float)model.glassWidth), -glm::min<float>(it->y + dy, (float)model.glassHeight) },
    };

    glm::vec2 uv[4] =
    {
      { pixSize,        pixSize },
      { 1.0f - pixSize, pixSize },
      { pixSize,        1.0f - pixSize },
      { 1.0f - pixSize, 1.0f - pixSize },
    };

    glm::vec3 color = Globals::ColorValues[it->color] * currentTrailAlpha;

    addVertex(origin + scale * verts[0], uv[0], Globals::dropTrailTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[1], uv[1], Globals::dropTrailTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[2], uv[2], Globals::dropTrailTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[1], uv[1], Globals::dropTrailTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[2], uv[2], Globals::dropTrailTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[3], uv[3], Globals::dropTrailTexIndex, color, 0.0f);

    for (int i = 0; i < DropTrail::sparkleQty; i++)
    {
      float sparkleX = it->x + it->sparkles[i].relX;
      float sparkleY = it->y - it->sparkles[i].relY * float(it->height) - it->sparkles[i].speed * it->getTrailProgress();
      const float sparkleSize = 0.1f;

      if (sparkleX < model.glassWidth - sparkleSize && sparkleY > 0.0f)
      {
        glm::vec2 verts[4] =
        {
          { sparkleX,               -sparkleY },
          { sparkleX + sparkleSize, -sparkleY },
          { sparkleX,               -sparkleY - sparkleSize },
          { sparkleX + sparkleSize, -sparkleY - sparkleSize },
        };

        addVertex(origin + scale * verts[0], uv[0], Globals::dropSparkleTexIndex, color, 0.0f);
        addVertex(origin + scale * verts[1], uv[1], Globals::dropSparkleTexIndex, color, 0.0f);
        addVertex(origin + scale * verts[2], uv[2], Globals::dropSparkleTexIndex, color, 0.0f);
        addVertex(origin + scale * verts[1], uv[1], Globals::dropSparkleTexIndex, color, 0.0f);
        addVertex(origin + scale * verts[2], uv[2], Globals::dropSparkleTexIndex, color, 0.0f);
        addVertex(origin + scale * verts[3], uv[3], Globals::dropSparkleTexIndex, color, 0.0f);
      }
    }
  }
}

void MainMesh::buildRowFlashesMesh()
{
  const glm::vec2 origin = Globals::glassPos;
  const float scale = Globals::glassSize.x / model.glassWidth;
  const float pixSize = Globals::mainArrayTexturePixelSize;

  float overallProgress = glm::clamp(float(getTimer() - model.rowsDeleteTimer) / model.rowsDeletionEffectTime, 0.0f, 1.0f);
  float mul = 1.0f - cos((overallProgress - 0.5f) * (overallProgress < 0.5f ? 0.5f : 2.0f) * (float)M_PI_2);
  float currentTrailAlpha = 1.25f - mul;
  float dx = 1.0f - mul * 3.0f;
  float dy = 0.4f - 0.9f * mul;

  for (std::vector<int>::iterator it = model.deletedRows.begin(); it != model.deletedRows.end(); ++it)
  {
    glm::vec2 verts[4] =
    {
      { 0.0f - dx, -*it + dy },
      { model.glassWidth + dx, -*it + dy },
      { 0.0f - dx, -*it - 1.0f - dy },
      { model.glassWidth + dx, -*it - 1.0f - dy },
    };

    glm::vec2 uv[4] =
    {
      { pixSize, pixSize },
      { 1.0f - pixSize, pixSize },
      { pixSize, 1.0f - pixSize },
      { 1.0f - pixSize, 1.0f - pixSize },
    };

    glm::vec3 color(currentTrailAlpha);

    addVertex(origin + scale * verts[0], uv[0], Globals::rowFlashTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[1], uv[1], Globals::rowFlashTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[2], uv[2], Globals::rowFlashTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[1], uv[1], Globals::rowFlashTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[2], uv[2], Globals::rowFlashTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[3], uv[3], Globals::rowFlashTexIndex, color, 0.0f);
  }

  if (!model.deletedRows.empty())
  {
    float shineProgress = glm::clamp(overallProgress / 0.75f, 0.0f, 1.0f);
    float ltX = (3.0f * shineProgress - 1.0f) * model.glassWidth;
    float ltY = 0.5f * (model.deletedRows.front() + model.deletedRows.back() + 1.0f);

    for (std::set<Model::CellCoord>::iterator vertGapsIt = model.deletedRowVertGaps.begin();
      vertGapsIt != model.deletedRowVertGaps.end();
      ++vertGapsIt)
    {
      float gapX = float(vertGapsIt->x);
      float gapY1 = float(vertGapsIt->y);
      float gapY2 = float(vertGapsIt->y + 1);
      float ltDX = gapX - ltX;
      float ltDY1 = gapY1 - ltY;
      float ltDY2 = gapY2 - ltY;
      const float leverRatio = 8.0f;
      float rayEndDX = ltDX * leverRatio;
      float rayEndDY1 = ltDY1 * leverRatio;
      float rayEndDY2 = ltDY2 * leverRatio;

      glm::vec2 uv[4] =
      {
        { 0.5f, pixSize },
        { pixSize, 1.0f - pixSize },
        { 1.0f - pixSize, 1.0f - pixSize },
      };

      float relDX = glm::clamp(fabs(ltDX / model.glassWidth), 0.0f, 1.0f);
      float alphaMul = 1.0f - 2.0f * fabs(0.5f - glm::clamp(relDX * relDX, 0.1f, 1.0f));

      glm::vec3 colors[3] =
      {
        glm::vec3(alphaMul),
        glm::vec3(0.0f),
        glm::vec3(0.0f),
      };

      glm::vec2 verts[3] =
      {
        { origin.x + scale * ltX, origin.y - scale * ltY },
        { origin.x + scale * (gapX + rayEndDX), origin.y - scale * (gapY1 + rayEndDY1) },
        { origin.x + scale * (gapX + rayEndDX), origin.y - scale * (gapY2 + rayEndDY2) },
      };

    //  for (int i = 1; i < 3; i++)
    //  {
    //    float mulX = 1.0f;
    //    float mulY = 1.0f;

    //    if (verts[i].x > Globals::gameBkPos.x + Globals::gameBkSize.x && verts[i].x - verts[0].x > VERY_SMALL_NUMBER)
    //    {
    //      mulY = (Globals::gameBkPos.x + Globals::gameBkSize.x - verts[0].x) / (verts[i].x - verts[0].x);
    //      verts[i].x = Globals::gameBkPos.x + Globals::gameBkSize.x;
    //      verts[i].y = verts[0].y + (verts[i].y - verts[0].y) * mulY;
    //    }

    //    if (verts[i].y < Globals::gameBkPos.y - Globals::gameBkSize.y && verts[i].y - verts[0].y > VERY_SMALL_NUMBER)
    //    {
    //      mulX = (Globals::gameBkPos.y - Globals::gameBkSize.y - verts[0].y) / (verts[i].y - verts[0].y);
    //      verts[i].y = Globals::gameBkPos.y - Globals::gameBkSize.y;
    //      verts[i].y = verts[0].x + (verts[i].x - verts[0].x) * mulX;
    //    }

    //    colors[i] = glm::vec3(alphaMul * (1.0f - mulX * mulY));
    //  }
      addVertex(verts[0], uv[0], Globals::rowShineRayTexIndex, colors[0], 0.0f);
      addVertex(verts[1], uv[1], Globals::rowShineRayTexIndex, colors[1], 0.0f);
      addVertex(verts[2], uv[2], Globals::rowShineRayTexIndex, colors[2], 0.0f);
    }

    //for (std::set<Model::CellCoord>::iterator vertGapsIt = model.deletedRowHorzGaps.begin();
    //  vertGapsIt != model.deletedRowHorzGaps.end();
    //  ++vertGapsIt)
    //{
    //  float gapX = (float)vertGapsIt->x;
    //  float gapY = -(float)vertGapsIt->y;
    //  float ltSourceDX1 = ltX - gapX;
    //  float ltSourceDX2 = ltX - gapX - 1.0f;
    //  float ltSourceDY = (ltY - gapY);
    //  float ltSourceMidDX = ltX - (gapX - 0.5f);
    //  const float leverRatio = 11.0f;
    //  float ltDestDX1 = -ltSourceDX1 * leverRatio;
    //  float ltDestDX2 = -ltSourceDX2 * leverRatio;
    //  float ltDestDY = -ltSourceDY * leverRatio;

    //  glm::vec2 uv[4] =
    //  {
    //    { 0.5f, pixSize },
    //    { pixSize, 1.0f - pixSize },
    //    { 1.0f - pixSize, 1.0f - pixSize },
    //  };

    //  float relDX = glm::clamp(fabs(ltSourceMidDX / model.glassWidth), 0.0f, 1.0f);
    //  float alphaMul = 1.0f - 2.0f * fabs(0.5f - glm::clamp(relDX * relDX, 0.1f, 1.0f));

    //  glm::vec3 color[3] =
    //  {
    //    glm::vec3(alphaMul),
    //    glm::vec3(0.0f),
    //    glm::vec3(0.0f),
    //  };

    //  glm::vec2 verts[3] =
    //  {
    //    { gapX + 0.5f + ltSourceMidDX, gapY + ltSourceDY },
    //    { gapX + 0.5f + ltDestDX1, gapY + ltDestDY },
    //    { gapX + 0.5f + ltDestDX2, gapY + ltDestDY },
    //  };

      //addVertex(origin + scale * verts[0], uv[0], Globals::rowShineRayTexIndex, color1, 0.0f);
      //addVertex(origin + scale * verts[1], uv[1], Globals::rowShineRayTexIndex, color2, 0.0f);
      //addVertex(origin + scale * verts[2], uv[2], Globals::rowShineRayTexIndex, color2, 0.0f);
    //}
  }
}

