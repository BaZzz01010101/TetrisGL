#include "Figure.h"
#include "Globals.h"
#include "Crosy.h"

std::vector<float> Figure::figureVertexBufferData;
std::vector<float> Figure::figureUVBufferData;
std::vector<float> Figure::shadowVertexBufferData;
std::vector<float> Figure::shadowUVBufferData;
std::vector<float> Figure::glowVertexBufferData;
std::vector<float> Figure::glowAlphaBufferData;

Program Figure::figureProg;
Shader Figure::figureVert(GL_VERTEX_SHADER);
Shader Figure::figureFrag(GL_FRAGMENT_SHADER);
Program Figure::glowProg;
Shader Figure::glowVert(GL_VERTEX_SHADER);
Shader Figure::glowFrag(GL_FRAGMENT_SHADER);

Figure::Figure() :
  dim(0),
  col(0),
  row(0),
  color(clRed),
  figureVertexBufferId(0),
  figureUVWBufferId(0),
  shadowVertexBufferId(0),
  shadowUVBufferId(0),
  glowVertexBufferId(0),
  glowAlphaBufferId(0),
  figureVertCount(0),
  shadowVertCount(0)
{
  Type type = Type(rand() * TYPE_COUNT / RAND_MAX);
  char * cdata = NULL;

  switch (type)
  {
  case typeI:
    dim = 4;
    color = clCyan;
    cdata = "0000111100000000";
    break;
  case typeJ:
    dim = 3;
    color = clBlue;
    cdata = "100111000";
    break;
  case typeL:
    dim = 3;
    color = clOrange;
    cdata = "001111000";
    break;
  case typeO:
    dim = 2;
    color = clYellow;
    cdata = "1111";
    break;
  case typeS:
    dim = 3;
    color = clGreen;
    cdata = "011110000";
    break;
  case typeT:
    dim = 3;
    color = clPurple;
    cdata = "010111000";
    break;
  case typeZ:
    dim = 3;
    color = clRed;
    cdata = "110011000";
    break;
  default: 
    assert(0);
  }

  assert(strlen(cdata) == dim * dim);

  for (const char * ptr = cdata, *end = cdata + dim * dim; ptr < end; ptr++)
    this->data.push_back(*ptr - '0');

  buildMeshes();
}
// data - is a string of '0' and '1'
Figure::Figure(int dim, Color color, const char * cdata) :
  dim(dim),
  col(0),
  row(0),
  color(color),
  figureVertexBufferId(0),
  figureUVWBufferId(0),
  shadowVertexBufferId(0),
  shadowUVBufferId(0),
  glowVertexBufferId(0),
  glowAlphaBufferId(0),
  figureVertCount(0),
  shadowVertCount(0)
{
  assert(strlen(cdata) == dim * dim);

  for (const char * ptr = cdata, * end = cdata + dim * dim; ptr < end; ptr++)
    this->data.push_back(*ptr - '0');

  buildMeshes();
}

Figure::~Figure()
{
  glDeleteBuffers(1, &figureVertexBufferId);
  glDeleteBuffers(1, &figureUVWBufferId);
  glDeleteBuffers(1, &shadowVertexBufferId);
  glDeleteBuffers(1, &shadowUVBufferId);
  glDeleteBuffers(1, &glowVertexBufferId);
  glDeleteBuffers(1, &glowAlphaBufferId);
}

void Figure::init()
{
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

  glowVert.compileFromString(
    "#version 330 core\n"
    "layout(location = 0) in vec2 vertexPos;"
    "layout(location = 1) in float vertexAlpha;"
    "uniform sampler2DArray tex;"
    "uniform float texLayer;"
    "uniform float scale;"
    "uniform vec2 pos;"
    "flat out vec3 color;"
    "out float alpha;"

    "void main()"
    "{"
    "  gl_Position = vec4(vertexPos * scale + pos, 0, 1);"
    "  color = texture(tex, vec3(0.5, 0.5, texLayer)).rgb;"
    "  alpha = vertexAlpha;"
    "}");

  glowFrag.compileFromString(
    "#version 330 core\n"
    "flat in vec3 color;"
    "in float alpha;"
    "out vec4 out_color;"

    "void main()"
    "{"
    "  out_color.rgb = color;"
    "  out_color.a = alpha;"
    "}");

  glowProg.attachShader(glowVert);
  glowProg.attachShader(glowFrag);
  glowProg.link();
  glowProg.use();
  glowProg.setUniform("tex", 0);
}

void Figure::buildMeshes()
{
  clearBuffersData();
  int index = 0;

  for (int iy = 0; iy < dim; iy++)
  for (int ix = 0; ix < dim; ix++)
  {
    float fx = float(ix);
    float fy = float(-iy);

    if (data[index])
    {
      enum {bmLeft = 1, bmLeftTop = 2, bmTop = 4, bmTopRight = 8, bmRight = 16, bmRightBottom = 32, bmBottom = 64, bmBottomLeft = 128};
      unsigned int bm = 0; // border mask

      if (!ix || !data[index - 1])                                    bm |= bmLeft;
      if (!ix || !iy || !data[index - dim - 1])                       bm |= bmLeftTop;
      if (!iy || !data[index - dim])                                  bm |= bmTop;
      if (!iy || ix == dim - 1 || !data[index - dim + 1])             bm |= bmTopRight;
      if (ix == dim - 1 || !data[index + 1])                          bm |= bmRight;
      if (ix == dim - 1 || iy == dim - 1 || !data[index + dim + 1])   bm |= bmRightBottom;
      if (iy == dim - 1 || !data[index + dim])                        bm |= bmBottom;
      if (!ix || iy == dim - 1 || !data[index + dim - 1])             bm |= bmBottomLeft;

      int blockArrayIndex;

      switch (bm & (bmLeft | bmLeftTop | bmTop))
      {
      case bmLeft | bmLeftTop | bmTop:
        blockArrayIndex = Globals::closedBlocksTexIndex;
        break;
      case 0:
        blockArrayIndex = Globals::openedBlocksTexIndex;
        break;
      case bmTop :
      case bmTop | bmLeftTop:
      case bmLeftTop:
        blockArrayIndex = Globals::horzBlocksTexIndex;
        break;
      case bmLeft :
      case bmLeft | bmLeftTop:
        blockArrayIndex = Globals::vertBlocksTexIndex;
        break;
      default:
        blockArrayIndex = Globals::openedBlocksTexIndex;
      }

      addFigureVertex(fx, fy, 0.0f, 0.0f, blockArrayIndex);
      addFigureVertex(fx, fy, 0.5f, 0.5f, blockArrayIndex);
      addFigureVertex(fx, fy, 0.0f, 0.5f, blockArrayIndex);

      switch (bm & (bmLeft | bmLeftTop | bmTop))
      {
      case bmLeft | bmLeftTop | bmTop:
        blockArrayIndex = Globals::closedBlocksTexIndex;
        break;
      case 0:
        blockArrayIndex = Globals::openedBlocksTexIndex;
        break;
      case bmTop:
      case bmTop | bmLeftTop:
        blockArrayIndex = Globals::horzBlocksTexIndex;
        break;
      case bmLeft :
      case bmLeft | bmLeftTop:
      case bmLeftTop:
        blockArrayIndex = Globals::vertBlocksTexIndex;
        break;
      default:
        blockArrayIndex = Globals::openedBlocksTexIndex;
      }

      addFigureVertex(fx, fy, 0.0f, 0.0f, blockArrayIndex);
      addFigureVertex(fx, fy, 0.5f, 0.0f, blockArrayIndex);
      addFigureVertex(fx, fy, 0.5f, 0.5f, blockArrayIndex);

      switch (bm & (bmTop | bmTopRight | bmRight))
      {
      case bmTop | bmTopRight | bmRight:
        blockArrayIndex = Globals::closedBlocksTexIndex;
        break;
      case 0:
        blockArrayIndex = Globals::openedBlocksTexIndex;
        break;
      case bmTop:
      case bmTop | bmTopRight:
        blockArrayIndex = Globals::horzBlocksTexIndex;
        break;
      case bmRight:
      case bmRight | bmTopRight:
      case bmTopRight:
        blockArrayIndex = Globals::vertBlocksTexIndex;
        break;
      default:
        blockArrayIndex = Globals::openedBlocksTexIndex;
      }

      addFigureVertex(fx, fy, 0.5f, 0.0f, blockArrayIndex);
      addFigureVertex(fx, fy, 1.0f, 0.0f, blockArrayIndex);
      addFigureVertex(fx, fy, 0.5f, 0.5f, blockArrayIndex);

      switch (bm & (bmRight | bmTopRight | bmTop))
      {
      case bmRight | bmTopRight | bmTop:
        blockArrayIndex = Globals::closedBlocksTexIndex;
        break;
      case 0:
        blockArrayIndex = Globals::openedBlocksTexIndex;
        break;
      case bmTop:
      case bmTop | bmTopRight:
      case bmTopRight:
        blockArrayIndex = Globals::horzBlocksTexIndex;
        break;
      case bmRight:
      case bmRight | bmTopRight:
        blockArrayIndex = Globals::vertBlocksTexIndex;
        break;
      default:
        blockArrayIndex = Globals::openedBlocksTexIndex;
      }

      addFigureVertex(fx, fy, 1.0f, 0.0f, blockArrayIndex);
      addFigureVertex(fx, fy, 1.0f, 0.5f, blockArrayIndex);
      addFigureVertex(fx, fy, 0.5f, 0.5f, blockArrayIndex);

      switch (bm & (bmRight | bmRightBottom | bmBottom))
      {
      case bmRight | bmRightBottom | bmBottom:
        blockArrayIndex = Globals::closedBlocksTexIndex;
        break;
      case 0:
        blockArrayIndex = Globals::openedBlocksTexIndex;
        break;
      case bmBottom:
      case bmBottom | bmRightBottom:
      case bmRightBottom:
        blockArrayIndex = Globals::horzBlocksTexIndex;
        break;
      case bmRight:
      case bmRight | bmRightBottom:
        blockArrayIndex = Globals::vertBlocksTexIndex;
        break;
      default:
        blockArrayIndex = Globals::openedBlocksTexIndex;
      }

      addFigureVertex(fx, fy, 0.5f, 0.5f, blockArrayIndex);
      addFigureVertex(fx, fy, 1.0f, 0.5f, blockArrayIndex);
      addFigureVertex(fx, fy, 1.0f, 1.0f, blockArrayIndex);

      switch (bm & (bmRight | bmRightBottom | bmBottom))
      {
      case bmRight | bmRightBottom | bmBottom:
        blockArrayIndex = Globals::closedBlocksTexIndex;
        break;
      case 0:
        blockArrayIndex = Globals::openedBlocksTexIndex;
        break;
      case bmBottom:
      case bmBottom | bmRightBottom:
        blockArrayIndex = Globals::horzBlocksTexIndex;
        break;
      case bmRight:
      case bmRight | bmRightBottom:
      case bmRightBottom:
        blockArrayIndex = Globals::vertBlocksTexIndex;
        break;
      default:
        blockArrayIndex = Globals::openedBlocksTexIndex;
      }

      addFigureVertex(fx, fy, 0.5f, 0.5f, blockArrayIndex);
      addFigureVertex(fx, fy, 1.0f, 1.0f, blockArrayIndex);
      addFigureVertex(fx, fy, 0.5f, 1.0f, blockArrayIndex);

      switch (bm & (bmBottom | bmBottomLeft | bmLeft))
      {
      case bmBottom | bmBottomLeft | bmLeft:
        blockArrayIndex = Globals::closedBlocksTexIndex;
        break;
      case 0:
        blockArrayIndex = Globals::openedBlocksTexIndex;
        break;
      case bmBottom:
      case bmBottom | bmBottomLeft:
        blockArrayIndex = Globals::horzBlocksTexIndex;
        break;
      case bmLeft:
      case bmLeft | bmBottomLeft:
      case bmBottomLeft:
        blockArrayIndex = Globals::vertBlocksTexIndex;
        break;
      default:
        blockArrayIndex = Globals::openedBlocksTexIndex;
      }

      addFigureVertex(fx, fy, 0.5f, 0.5f, blockArrayIndex);
      addFigureVertex(fx, fy, 0.5f, 1.0f, blockArrayIndex);
      addFigureVertex(fx, fy, 0.0f, 1.0f, blockArrayIndex);

      switch (bm & (bmLeft | bmBottomLeft | bmBottom))
      {
      case bmLeft | bmBottomLeft | bmBottom:
        blockArrayIndex = Globals::closedBlocksTexIndex;
        break;
      case 0:
        blockArrayIndex = Globals::openedBlocksTexIndex;
        break;
      case bmBottom:
      case bmBottom | bmBottomLeft:
      case bmBottomLeft:
        blockArrayIndex = Globals::horzBlocksTexIndex;
        break;
      case bmLeft:
      case bmLeft | bmBottomLeft:
        blockArrayIndex = Globals::vertBlocksTexIndex;
        break;
      default:
        blockArrayIndex = Globals::openedBlocksTexIndex;
      }

      addFigureVertex(fx, fy, 0.5f, 0.5f, blockArrayIndex);
      addFigureVertex(fx, fy, 0.0f, 1.0f, blockArrayIndex);
      addFigureVertex(fx, fy, 0.0f, 0.5f, blockArrayIndex);

      const float shadowWidth = 0.2f;

      if (bm & bmBottom)
      {
        const float ru = 0.9f;
        float lu;

        if (bm & bmLeft)
          lu = 0.0f;
        else
          lu = 0.5f;

        addShadowVertex(fx + 0.0f, fy - 1.0f, lu, 0.0f);
        addShadowVertex(fx + 1.0f, fy - 1.0f, ru, 0.0f);

        if (bm & bmRightBottom)
        {
          addShadowVertex(fx + 1.0f + shadowWidth, fy - 1.0f - shadowWidth, ru, 1.0f);
          addShadowVertex(fx + 1.0f + shadowWidth, fy - 1.0f - shadowWidth, ru, 1.0f);
        }
        else
        {
          addShadowVertex(fx + 1.0f, fy - 1.0f - shadowWidth, ru, 1.0);
          addShadowVertex(fx + 1.0f, fy - 1.0f - shadowWidth, ru, 1.0);
        }

        addShadowVertex(fx + 0.0f, fy - 1.0f, lu, 0.0f);

        if (bm & bmBottomLeft)
          addShadowVertex(fx + shadowWidth, fy - 1.0f - shadowWidth, lu, 1.0);
        else
          addShadowVertex(fx + 0.0f, fy - 1.0f - shadowWidth, lu, 1.0f);
      }

      if (bm & bmRight)
      {
        const float bu = 0.9f;
        float tu;

        if (bm & bmTop)
          tu = 0.0f;
        else
          tu = 0.5f;

        addShadowVertex(fx + 1.0f, fy - 0.0f, tu, 0.0f);
        addShadowVertex(fx + 1.0f, fy - 1.0f, bu, 0.0f);

        if (bm & bmRightBottom)
        {
          addShadowVertex(fx + 1.0f + shadowWidth, fy - 1.0f - shadowWidth, bu, 1.0f);
          addShadowVertex(fx + 1.0f + shadowWidth, fy - 1.0f - shadowWidth, bu, 1.0f);
        }
        else
        {
          addShadowVertex(fx + 1.0f + shadowWidth, fy - 1.0f, bu, 1.0f);
          addShadowVertex(fx + 1.0f + shadowWidth, fy - 1.0f, bu, 1.0f);
        }

        addShadowVertex(fx + 1.0f, fy + 0.0f, tu, 0.0f);

        if (bm & bmTopRight)
          addShadowVertex(fx + 1.0f + shadowWidth, fy - shadowWidth, tu, 1.0f);
        else
          addShadowVertex(fx + 1.0f + shadowWidth, fy - 0.0f, tu, 1.0f);
      }


      const float glowWidth = 0.5f;
      const float glowBias = 0.0f / 64.0f;
      const float maxAlpha = 0.2f;
      const float minAlpha = 0.0f;
      float dx = 0.0f;
      float dy = 0.0f;

      if (bm & bmLeft)
      {
        dx = -glowWidth;

        addGlowVertex(fx + 0.0f + glowBias, fy + 0.0f - glowBias, maxAlpha);
        addGlowVertex(fx + 0.0f + glowBias, fy - 1.0f + glowBias, maxAlpha);

        switch (bm & (bmBottom | bmBottomLeft))
        {
        case bmBottom | bmBottomLeft:
          dy = -1.0f - glowWidth;
          break;
        case bmBottomLeft:
          dy = -1.0f;
          break;
        default:
          dy = -1.0f + glowWidth;
        }

        addGlowVertex(fx + dx, fy + dy, minAlpha);

        addGlowVertex(fx + 0.0f + glowBias, fy + 0.0f - glowBias, maxAlpha);
        addGlowVertex(fx + dx, fy + dy, minAlpha);

        switch (bm & (bmTop | bmLeftTop))
        {
        case bmTop | bmLeftTop:
          dy = 0.0f + glowWidth;
          break;
        case bmLeftTop:
          dy = 0.0f;
          break;
        default:
          dy = 0.0f - glowWidth;
        }

        addGlowVertex(fx + dx, fy + dy, minAlpha);
      }

      if (bm & bmTop)
      {
        dy = glowWidth;

        addGlowVertex(fx + 1.0f - glowBias, fy + 0.0f - glowBias, maxAlpha);
        addGlowVertex(fx + 0.0f + glowBias, fy + 0.0f - glowBias, maxAlpha);

        switch (bm & (bmLeft | bmLeftTop))
        {
        case bmLeft | bmLeftTop:
          dx = - glowWidth;
          break;
        case bmLeftTop:
          dx = 0.0f;
          break;
        default:
          dx = glowWidth;
        }

        addGlowVertex(fx + dx, fy + dy, minAlpha);

        addGlowVertex(fx + 1.0f - glowBias, fy + 0.0f - glowBias, maxAlpha);
        addGlowVertex(fx + dx, fy + dy, minAlpha);

        switch (bm & (bmRight | bmTopRight))
        {
        case bmRight | bmTopRight:
          dx = 1.0f + glowWidth;
          break;
        case bmTopRight:
          dx = 1.0f;
          break;
        default:
          dx = 1.0f - glowWidth;
        }

        addGlowVertex(fx + dx, fy + dy, minAlpha);
      }

      if (bm & bmRight)
      {
        dx = 1.0f + glowWidth;

        addGlowVertex(fx + 1.0f - glowBias, fy + 0.0f - glowBias, maxAlpha);
        addGlowVertex(fx + 1.0f - glowBias, fy - 1.0f + glowBias, maxAlpha);

        switch (bm & (bmTop | bmTopRight))
        {
        case bmTop | bmTopRight:
          dy = glowWidth;
          break;
        case bmTopRight:
          dy = 0.0f;
          break;
        default:
          dy = -glowWidth;
        }

        addGlowVertex(fx + dx, fy + dy, minAlpha);

        addGlowVertex(fx + 1.0f - glowBias, fy - 1.0f + glowBias, maxAlpha);
        addGlowVertex(fx + dx, fy + dy, minAlpha);

        switch (bm & (bmBottom | bmRightBottom))
        {
        case bmBottom | bmRightBottom:
          dy = -1.0f - glowWidth;
          break;
        case bmRightBottom:
          dy = -1.0f;
          break;
        default:
          dy = -1.0f + glowWidth;
        }

        addGlowVertex(fx + dx, fy + dy, minAlpha);
      }

      if (bm & bmBottom)
      {
        dy = -1.0f - glowWidth;

        addGlowVertex(fx + 0.0f + glowBias, fy - 1.0f + glowBias, maxAlpha);
        addGlowVertex(fx + 1.0f - glowBias, fy - 1.0f + glowBias, maxAlpha);

        switch (bm & (bmRight | bmRightBottom))
        {
        case bmRight | bmRightBottom:
          dx = 1.0f + glowWidth;
          break;
        case bmRightBottom:
          dx = 1.0f;
          break;
        default:
          dx = 1.0f - glowWidth;
        }

        addGlowVertex(fx + dx, fy + dy, minAlpha);

        addGlowVertex(fx + 0.0f + glowBias, fy - 1.0f + glowBias, maxAlpha);
        addGlowVertex(fx + dx, fy + dy, minAlpha);

        switch (bm & (bmLeft | bmBottomLeft))
        {
        case bmLeft | bmBottomLeft:
          dx = -glowWidth;
          break;
        case bmBottomLeft:
          dx = 0.0f;
          break;
        default:
          dx = glowWidth;
        }

        addGlowVertex(fx + dx, fy + dy, minAlpha);
      }

    }

    index++;
  }

  if (!figureVertexBufferId) {
    glGenBuffers(1, &figureVertexBufferId);
    assert(!checkGlErrors());
  }

  glBindBuffer(GL_ARRAY_BUFFER, figureVertexBufferId);
  assert(!checkGlErrors());

  glBufferData(GL_ARRAY_BUFFER, figureVertexBufferData.size() * sizeof(float), &figureVertexBufferData.front(), GL_STATIC_DRAW);
  assert(!checkGlErrors());


  if (!figureUVWBufferId) {
    glGenBuffers(1, &figureUVWBufferId);
    assert(!checkGlErrors());
  }

  glBindBuffer(GL_ARRAY_BUFFER, figureUVWBufferId);
  assert(!checkGlErrors());

  glBufferData(GL_ARRAY_BUFFER, figureUVBufferData.size() * sizeof(float), &figureUVBufferData.front(), GL_STATIC_DRAW);
  assert(!checkGlErrors());


  if (!shadowVertexBufferId) {
    glGenBuffers(1, &shadowVertexBufferId);
    assert(!checkGlErrors());
  }

  glBindBuffer(GL_ARRAY_BUFFER, shadowVertexBufferId);
  assert(!checkGlErrors());

  glBufferData(GL_ARRAY_BUFFER, shadowVertexBufferData.size() * sizeof(float), &shadowVertexBufferData.front(), GL_STATIC_DRAW);
  assert(!checkGlErrors());


  if (!shadowUVBufferId) {
    glGenBuffers(1, &shadowUVBufferId);
    assert(!checkGlErrors());
  }

  glBindBuffer(GL_ARRAY_BUFFER, shadowUVBufferId);
  assert(!checkGlErrors());

  glBufferData(GL_ARRAY_BUFFER, shadowUVBufferData.size() * sizeof(float), &shadowUVBufferData.front(), GL_STATIC_DRAW);
  assert(!checkGlErrors());


  if (!glowVertexBufferId) {
    glGenBuffers(1, &glowVertexBufferId);
    assert(!checkGlErrors());
  }

  glBindBuffer(GL_ARRAY_BUFFER, glowVertexBufferId);
  assert(!checkGlErrors());

  glBufferData(GL_ARRAY_BUFFER, glowVertexBufferData.size() * sizeof(float), &glowVertexBufferData.front(), GL_STATIC_DRAW);
  assert(!checkGlErrors());


  if (!glowAlphaBufferId) {
    glGenBuffers(1, &glowAlphaBufferId);
    assert(!checkGlErrors());
  }

  glBindBuffer(GL_ARRAY_BUFFER, glowAlphaBufferId);
  assert(!checkGlErrors());

  glBufferData(GL_ARRAY_BUFFER, glowAlphaBufferData.size() * sizeof(float), &glowAlphaBufferData.front(), GL_STATIC_DRAW);
  assert(!checkGlErrors());
}

void Figure::clearBuffersData()
{
  figureVertexBufferData.clear();
  figureUVBufferData.clear();
  shadowVertexBufferData.clear();
  shadowUVBufferData.clear();
  glowVertexBufferData.clear();
  glowAlphaBufferData.clear();

  figureVertCount = 0;
  shadowVertCount = 0;
  glowVertCount = 0;
}

void Figure::addFigureVertex(float sqx, float sqy, float u, float v, int blockArrayIndex)
{
  figureVertexBufferData.push_back(sqx + u);
  figureVertexBufferData.push_back(sqy - v);
  figureUVBufferData.push_back(u);
  figureUVBufferData.push_back(v);
  figureUVBufferData.push_back(float(blockArrayIndex + color));

  figureVertCount++;
}

void Figure::addShadowVertex(float x, float y, float u, float v)
{
  shadowVertexBufferData.push_back(x);
  shadowVertexBufferData.push_back(y);
  shadowUVBufferData.push_back(u);
  shadowUVBufferData.push_back(v);
  shadowUVBufferData.push_back(float(Globals::shadowTexIndex));

  shadowVertCount++;
}

void Figure::addGlowVertex(float x, float y, float alpha)
{
  glowVertexBufferData.push_back(x);
  glowVertexBufferData.push_back(y);
  glowAlphaBufferData.push_back(alpha);

  glowVertCount++;
}

void Figure::rotate(Rotation rot)
{
  std::vector<int> curData = data;

  if (rot == rotLeft)
  {
    for (int x = 0; x < dim; x++)
    for (int y = 0; y < dim; y++)
    {
      data[x + y * dim] = curData[(dim - y - 1) + x * dim];
    }
  }
  else if (rot == rotRight)
  {
    for (int x = 0; x < dim; x++)
    for (int y = 0; y < dim; y++)
    {
      data[x + y * dim] = curData[y + (dim - x - 1) * dim];
    }
  }
}

void Figure::setScale(float scale)
{
  figureProg.use();
  figureProg.setUniform("scale", scale);
  glowProg.use();
  glowProg.setUniform("scale", scale);
}

void Figure::drawFigure(float x, float y)
{
  figureProg.use();
  figureProg.setUniform("pos", glm::vec2(x, y));
  glEnableVertexAttribArray(0);
  assert(!checkGlErrors());
  glEnableVertexAttribArray(1);
  assert(!checkGlErrors());


  glBindBuffer(GL_ARRAY_BUFFER, figureVertexBufferId);
  assert(!checkGlErrors());
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!checkGlErrors());

  glBindBuffer(GL_ARRAY_BUFFER, figureUVWBufferId);
  assert(!checkGlErrors());
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!checkGlErrors());

  glDrawArrays(GL_TRIANGLES, 0, figureVertCount);
  assert(!checkGlErrors());


  glDisableVertexAttribArray(0);
  assert(!checkGlErrors());
  glDisableVertexAttribArray(1);
  assert(!checkGlErrors());
}

void Figure::drawShadow(float x, float y)
{
  figureProg.use();
  figureProg.setUniform("pos", glm::vec2(x, y));

  glEnableVertexAttribArray(0);
  assert(!checkGlErrors());
  glEnableVertexAttribArray(1);
  assert(!checkGlErrors());


  glBindBuffer(GL_ARRAY_BUFFER, shadowVertexBufferId);
  assert(!checkGlErrors());
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!checkGlErrors());

  glBindBuffer(GL_ARRAY_BUFFER, shadowUVBufferId);
  assert(!checkGlErrors());
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!checkGlErrors());

  glDrawArrays(GL_TRIANGLES, 0, shadowVertCount);
  assert(!checkGlErrors());


  glDisableVertexAttribArray(0);
  assert(!checkGlErrors());
  glDisableVertexAttribArray(1);
  assert(!checkGlErrors());
}

void Figure::drawGlow(float x, float y)
{
  glowProg.use();
  glowProg.setUniform("pos", glm::vec2(x, y));
  glowProg.setUniform("texLayer", float(Globals::openedBlocksTexIndex + color));

  glEnableVertexAttribArray(0);
  assert(!checkGlErrors());
  glEnableVertexAttribArray(1);
  assert(!checkGlErrors());

  glBindBuffer(GL_ARRAY_BUFFER, glowVertexBufferId);
  assert(!checkGlErrors());
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!checkGlErrors());

  glBindBuffer(GL_ARRAY_BUFFER, glowAlphaBufferId);
  assert(!checkGlErrors());
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!checkGlErrors());

  glDrawArrays(GL_TRIANGLES, 0, glowVertCount);
  assert(!checkGlErrors());

  glDisableVertexAttribArray(0);
  assert(!checkGlErrors());
  glDisableVertexAttribArray(1);
  assert(!checkGlErrors());
}
