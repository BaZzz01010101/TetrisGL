#include "Figure.h"
#include "Crosy.h"

GLuint Figure::textureId = 0;
Program Figure::figureProg;
Shader Figure::figureVert(GL_VERTEX_SHADER);
Shader Figure::figureFrag(GL_FRAGMENT_SHADER);
Program Figure::glowProg;
Shader Figure::glowVert(GL_VERTEX_SHADER);
Shader Figure::glowFrag(GL_FRAGMENT_SHADER);
glm::vec2 Figure::screenScale(1.0f);

// data - is a string of '0' and '1'
Figure::Figure(int dim, Color color, const char * data) :
  dim(dim),
  color(color),
  figureVertexBufferId(0),
  figureUVBufferId(0),
  shadowVertexBufferId(0),
  shadowUVBufferId(0),
  glowVertexBufferId(0),
  glowUVBufferId(0),
  glowAlphaBufferId(0),
  figureVertCount(0),
  shadowVertCount(0)
{
  assert(strlen(data) == dim * dim);

  for (const char * ptr = data, * end = data + dim * dim; ptr < end; ptr++)
    this->data.push_back(*ptr - '0');

  buildMeshes();
}

Figure::~Figure()
{
  glDeleteBuffers(1, &figureVertexBufferId);
  glDeleteBuffers(1, &figureUVBufferId);
  glDeleteBuffers(1, &shadowVertexBufferId);
  glDeleteBuffers(1, &shadowUVBufferId);
  glDeleteBuffers(1, &glowVertexBufferId);
  glDeleteBuffers(1, &glowUVBufferId);
  glDeleteBuffers(1, &glowAlphaBufferId);
}

void Figure::init()
{
  std::string texPath = Crosy::getExePath() + "\\textures\\blocks.png";
  textureId = SOIL_load_OGL_texture(texPath.c_str(), 0, 0, 0);
  assert(textureId);
  assert(!glGetError());

  figureVert.compileFromString(
    "#version 330 core\n"
    "layout(location = 0) in vec2 vertexPos;"
    "layout(location = 1) in vec2 vertexUV;"
    "uniform vec2 screenScale;"
    "uniform float sqSize;"
    "uniform vec2 pos;"
    "out vec2 uv;"

    "void main()"
    "{"
    "  gl_Position = vec4(screenScale * (vertexPos * sqSize + pos), 0, 1);"
    "  uv = vertexUV;"
    "}");
  assert(!figureVert.isError());

  figureFrag.compileFromString(
    "#version 330 core\n"
    "uniform sampler2D tex;"
    "in vec2 uv;"
    "out vec4 out_color;"

    "void main()"
    "{"
    "  out_color = texture(tex, uv).rgba;"
    "}");
  assert(!figureFrag.isError());

  figureProg.attachShader(figureVert);
  assert(!figureProg.isError());

  figureProg.attachShader(figureFrag);
  assert(!figureProg.isError());

  figureProg.link();
  assert(!figureProg.isError());

  figureProg.use();
  assert(!figureProg.isError());

  GLuint texId = glGetUniformLocation(figureProg.getId(), "tex");
  assert(!glGetError());

  glUniform1i(texId, 0);
  assert(!glGetError());

  glowVert.compileFromString(
    "#version 330 core\n"
    "layout(location = 0) in vec2 vertexPos;"
    "layout(location = 1) in vec2 vertexUV;"
    "layout(location = 2) in float vertexAlpha;"
    "uniform vec2 screenScale;"
    "uniform float sqSize;"
    "uniform vec2 pos;"
    "uniform sampler2D tex;"
    "flat out vec3 color;"
    "out float alpha;"

    "void main()"
    "{"
    "  gl_Position = vec4(screenScale * (vertexPos * sqSize + pos), 0, 1);"
    "  color = texture(tex, vertexUV).rgb;"
    "  alpha = vertexAlpha;"
    "}");
  assert(!glowVert.isError());

  glowFrag.compileFromString(
    "#version 330 core\n"
    "flat in vec3 color;"
    "in float alpha;"
    "out vec4 out_color;"

    "void main()"
    "{"
    "  out_color = vec4(color, alpha);"
    "}");
  assert(!glowFrag.isError());

  glowProg.attachShader(glowVert);
  assert(!glowProg.isError());

  glowProg.attachShader(glowFrag);
  assert(!glowProg.isError());

  glowProg.link();
  assert(!glowProg.isError());

  texId = glGetUniformLocation(glowProg.getId(), "tex");
  assert(!glGetError());

  glUniform1i(texId, 0);
  assert(!glGetError());
}

void Figure::buildMeshes()
{
  clearVertexBuffersData();
  int index = 0;

  for (int iy = 0; iy < dim; iy++)
  for (int ix = 0; ix < dim; ix++)
  {
    const float openedBlockDV = 0.0f;
    const float closedBlockDV = 0.125f;
    const float horzBlockDV = 0.25f;
    const float vertBlockDV = 0.375f;
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

      float dv;

      switch (bm & (bmLeft | bmLeftTop | bmTop))
      {
      case bmLeft | bmLeftTop | bmTop:
        dv = closedBlockDV;
        break;
      case 0:
        dv = openedBlockDV;
        break;
      case bmTop :
      case bmTop | bmLeftTop:
      case bmLeftTop:
        dv = horzBlockDV;
        break;
      case bmLeft :
      case bmLeft | bmLeftTop:
        dv = vertBlockDV;
        break;
      default:
        dv = openedBlockDV;
      }

      addFigureVertex(fx, fy, 0.0f, 0.0f, dv);
      addFigureVertex(fx, fy, 0.5f, -0.5f, dv);
      addFigureVertex(fx, fy, 0.0f, -0.5f, dv);

      switch (bm & (bmLeft | bmLeftTop | bmTop))
      {
      case bmLeft | bmLeftTop | bmTop:
        dv = closedBlockDV;
        break;
      case 0:
        dv = openedBlockDV;
        break;
      case bmTop:
      case bmTop | bmLeftTop:
        dv = horzBlockDV;
        break;
      case bmLeft :
      case bmLeft | bmLeftTop:
      case bmLeftTop:
        dv = vertBlockDV;
        break;
      default:
        dv = openedBlockDV;
      }

      addFigureVertex(fx, fy, 0.0f, 0.0f, dv);
      addFigureVertex(fx, fy, 0.5f, 0.0f, dv);
      addFigureVertex(fx, fy, 0.5f, -0.5f, dv);

      switch (bm & (bmTop | bmTopRight | bmRight))
      {
      case bmTop | bmTopRight | bmRight:
        dv = closedBlockDV;
        break;
      case 0:
        dv = openedBlockDV;
        break;
      case bmTop:
      case bmTop | bmTopRight:
        dv = horzBlockDV;
        break;
      case bmRight:
      case bmRight | bmTopRight:
      case bmTopRight:
        dv = vertBlockDV;
        break;
      default:
        dv = openedBlockDV;
      }

      addFigureVertex(fx, fy, 0.5f, 0.0f, dv);
      addFigureVertex(fx, fy, 1.0f, 0.0f, dv);
      addFigureVertex(fx, fy, 0.5f, -0.5f, dv);

      switch (bm & (bmRight | bmTopRight | bmTop))
      {
      case bmRight | bmTopRight | bmTop:
        dv = closedBlockDV;
        break;
      case 0:
        dv = openedBlockDV;
        break;
      case bmTop:
      case bmTop | bmTopRight:
      case bmTopRight:
        dv = horzBlockDV;
        break;
      case bmRight:
      case bmRight | bmTopRight:
        dv = vertBlockDV;
        break;
      default:
        dv = openedBlockDV;
      }

      addFigureVertex(fx, fy, 1.0f, 0.0f, dv);
      addFigureVertex(fx, fy, 1.0f, -0.5f, dv);
      addFigureVertex(fx, fy, 0.5f, -0.5f, dv);

      switch (bm & (bmRight | bmRightBottom | bmBottom))
      {
      case bmRight | bmRightBottom | bmBottom:
        dv = closedBlockDV;
        break;
      case 0:
        dv = openedBlockDV;
        break;
      case bmBottom:
      case bmBottom | bmRightBottom:
      case bmRightBottom:
        dv = horzBlockDV;
        break;
      case bmRight:
      case bmRight | bmRightBottom:
        dv = vertBlockDV;
        break;
      default:
        dv = openedBlockDV;
      }

      addFigureVertex(fx, fy, 0.5f, -0.5f, dv);
      addFigureVertex(fx, fy, 1.0f, -0.5f, dv);
      addFigureVertex(fx, fy, 1.0f, -1.0f, dv);

      switch (bm & (bmRight | bmRightBottom | bmBottom))
      {
      case bmRight | bmRightBottom | bmBottom:
        dv = closedBlockDV;
        break;
      case 0:
        dv = openedBlockDV;
        break;
      case bmBottom:
      case bmBottom | bmRightBottom:
        dv = horzBlockDV;
        break;
      case bmRight:
      case bmRight | bmRightBottom:
      case bmRightBottom:
        dv = vertBlockDV;
        break;
      default:
        dv = openedBlockDV;
      }

      addFigureVertex(fx, fy, 0.5f, -0.5f, dv);
      addFigureVertex(fx, fy, 1.0f, -1.0f, dv);
      addFigureVertex(fx, fy, 0.5f, -1.0f, dv);

      switch (bm & (bmBottom | bmBottomLeft | bmLeft))
      {
      case bmBottom | bmBottomLeft | bmLeft:
        dv = closedBlockDV;
        break;
      case 0:
        dv = openedBlockDV;
        break;
      case bmBottom:
      case bmBottom | bmBottomLeft:
        dv = horzBlockDV;
        break;
      case bmLeft:
      case bmLeft | bmBottomLeft:
      case bmBottomLeft:
        dv = vertBlockDV;
        break;
      default:
        dv = openedBlockDV;
      }

      addFigureVertex(fx, fy, 0.5f, -0.5f, dv);
      addFigureVertex(fx, fy, 0.5f, -1.0f, dv);
      addFigureVertex(fx, fy, 0.0f, -1.0f, dv);

      switch (bm & (bmLeft | bmBottomLeft | bmBottom))
      {
      case bmLeft | bmBottomLeft | bmBottom:
        dv = closedBlockDV;
        break;
      case 0:
        dv = openedBlockDV;
        break;
      case bmBottom:
      case bmBottom | bmBottomLeft:
      case bmBottomLeft:
        dv = horzBlockDV;
        break;
      case bmLeft:
      case bmLeft | bmBottomLeft:
        dv = vertBlockDV;
        break;
      default:
        dv = openedBlockDV;
      }

      addFigureVertex(fx, fy, 0.5f, -0.5f, dv);
      addFigureVertex(fx, fy, 0.0f, -1.0f, dv);
      addFigureVertex(fx, fy, 0.0f, -0.5f, dv);

      const float shadowWidth = 0.2f;

      if (bm & bmBottom)
      {
        const float ru = 511.0f / 512.0f;
        float lu;

        if (bm & bmLeft)
          lu = 0.877f;
        else
          lu = 0.9375f;

        addShadowVertex(fx, fy, 0.0f, -1.0f, lu, 0.0f);
        addShadowVertex(fx, fy, 1.0f, -1.0f, ru, 0.0f);

        if (bm & bmRightBottom)
        {
          addShadowVertex(fx, fy, 1.0f + shadowWidth, -1.0f - shadowWidth, ru, 0.125f);
          addShadowVertex(fx, fy, 1.0f + shadowWidth, -1.0f - shadowWidth, ru, 0.125f);
        }
        else
        {
          addShadowVertex(fx, fy, 1.0f, - 1.0f - shadowWidth, ru, 0.125f);
          addShadowVertex(fx, fy, 1.0f, - 1.0f - shadowWidth, ru, 0.125f);
        }

        addShadowVertex(fx, fy, 0.0f, -1.0f, lu, 0.0f);

        if (bm & bmBottomLeft)
          addShadowVertex(fx + shadowWidth, fy, 0.0f, -1.0f - shadowWidth, lu, 0.125f);
        else
          addShadowVertex(fx, fy, 0.0f, -1.0f - shadowWidth, lu, 0.125f);
      }

      if (bm & bmRight)
      {
        const float bu = 511.0f / 512.0f;
        float tu;

        if (bm & bmTop)
          tu = 0.877f;
        else
          tu = 0.9375f;

        addShadowVertex(fx, fy, 1.0f, 0.0f, tu, 0.0f);
        addShadowVertex(fx, fy, 1.0f, -1.0f, bu, 0.0f);

        if (bm & bmRightBottom)
        {
          addShadowVertex(fx, fy, 1.0f + shadowWidth, -1.0f - shadowWidth, bu, 0.125f);
          addShadowVertex(fx, fy, 1.0f + shadowWidth, -1.0f - shadowWidth, bu, 0.125f);
        }
        else
        {
          addShadowVertex(fx, fy, 1.0f + shadowWidth, -1.0f, bu, 0.125f);
          addShadowVertex(fx, fy, 1.0f + shadowWidth, -1.0f, bu, 0.125f);
        }

        addShadowVertex(fx, fy, 1.0f, 0.0f, tu, 0.0f);

        if (bm & bmTopRight)
          addShadowVertex(fx, fy, 1.0f + shadowWidth, -shadowWidth, tu, 0.125f);
        else
          addShadowVertex(fx, fy, 1.0f + shadowWidth, 0.0f, tu, 0.125f);
      }


      const float glowWidth = 0.5f;
      const float maxAlpha = 0.2f;
      const float minAlpha = 0.0f;
      float dx = 0.0f;
      float dy = 0.0f;

      if (bm & bmLeft)
      {
        dx = -glowWidth;

        addGlowVertex(fx, fy, 0.0f, 0.0f, maxAlpha);
        addGlowVertex(fx, fy, 0.0f, -1.0f, maxAlpha);

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

        addGlowVertex(fx, fy, dx, dy, minAlpha);

        addGlowVertex(fx, fy, 0.0f, 0.0f, maxAlpha);
        addGlowVertex(fx, fy, dx, dy, minAlpha);

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

        addGlowVertex(fx, fy, dx, dy, minAlpha);
      }

      if (bm & bmTop)
      {
        dy = glowWidth;

        addGlowVertex(fx, fy, 1.0f, 0.0f, maxAlpha);
        addGlowVertex(fx, fy, 0.0f, 0.0f, maxAlpha);

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

        addGlowVertex(fx, fy, dx, dy, minAlpha);

        addGlowVertex(fx, fy, 1.0f, 0.0f, maxAlpha);
        addGlowVertex(fx, fy, dx, dy, minAlpha);

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

        addGlowVertex(fx, fy, dx, dy, minAlpha);
      }

      if (bm & bmRight)
      {
        dx = 1.0f + glowWidth;

        addGlowVertex(fx, fy, 1.0f, 0.0f, maxAlpha);
        addGlowVertex(fx, fy, 1.0f, -1.0f, maxAlpha);

        switch (bm & (bmTop | bmTopRight))
        {
        case bmTop | bmTopRight:
          dy = glowWidth;
          break;
        case bmTopRight:
          dy = 0.0f;
          break;
        default:
          dy = glowWidth;
        }

        addGlowVertex(fx, fy, dx, dy, minAlpha);

        addGlowVertex(fx, fy, 1.0f, -1.0f, maxAlpha);
        addGlowVertex(fx, fy, dx, dy, minAlpha);

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

        addGlowVertex(fx, fy, dx, dy, minAlpha);
      }

      if (bm & bmBottom)
      {
        dy = -1.0f - glowWidth;

        addGlowVertex(fx, fy, 0.0f, -1.0f, maxAlpha);
        addGlowVertex(fx, fy, 1.0f, -1.0f, maxAlpha);

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

        addGlowVertex(fx, fy, dx, dy, minAlpha);

        addGlowVertex(fx, fy, 0.0f, -1.0f, maxAlpha);
        addGlowVertex(fx, fy, dx, dy, minAlpha);

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

        addGlowVertex(fx, fy, dx, dy, minAlpha);
      }

    }

    index++;
  }

  if (!figureVertexBufferId) {
    glGenBuffers(1, &figureVertexBufferId);
    assert(!glGetError());
  }

  glBindBuffer(GL_ARRAY_BUFFER, figureVertexBufferId);
  assert(!glGetError());

  glBufferData(GL_ARRAY_BUFFER, figureVertexBufferData.size() * sizeof(float), &figureVertexBufferData.front(), GL_STATIC_DRAW);
  assert(!glGetError());


  if (!figureUVBufferId) {
    glGenBuffers(1, &figureUVBufferId);
    assert(!glGetError());
  }

  glBindBuffer(GL_ARRAY_BUFFER, figureUVBufferId);
  assert(!glGetError());

  glBufferData(GL_ARRAY_BUFFER, figureUVBufferData.size() * sizeof(float), &figureUVBufferData.front(), GL_STATIC_DRAW);
  assert(!glGetError());


  if (!shadowVertexBufferId) {
    glGenBuffers(1, &shadowVertexBufferId);
    assert(!glGetError());
  }

  glBindBuffer(GL_ARRAY_BUFFER, shadowVertexBufferId);
  assert(!glGetError());

  glBufferData(GL_ARRAY_BUFFER, shadowVertexBufferData.size() * sizeof(float), &shadowVertexBufferData.front(), GL_STATIC_DRAW);
  assert(!glGetError());


  if (!shadowUVBufferId) {
    glGenBuffers(1, &shadowUVBufferId);
    assert(!glGetError());
  }

  glBindBuffer(GL_ARRAY_BUFFER, shadowUVBufferId);
  assert(!glGetError());

  glBufferData(GL_ARRAY_BUFFER, shadowUVBufferData.size() * sizeof(float), &shadowUVBufferData.front(), GL_STATIC_DRAW);
  assert(!glGetError());


  if (!glowVertexBufferId) {
    glGenBuffers(1, &glowVertexBufferId);
    assert(!glGetError());
  }

  glBindBuffer(GL_ARRAY_BUFFER, glowVertexBufferId);
  assert(!glGetError());

  glBufferData(GL_ARRAY_BUFFER, glowVertexBufferData.size() * sizeof(float), &glowVertexBufferData.front(), GL_STATIC_DRAW);
  assert(!glGetError());


  if (!glowUVBufferId) {
    glGenBuffers(1, &glowUVBufferId);
    assert(!glGetError());
  }

  glBindBuffer(GL_ARRAY_BUFFER, glowUVBufferId);
  assert(!glGetError());

  glBufferData(GL_ARRAY_BUFFER, glowUVBufferData.size() * sizeof(float), &glowUVBufferData.front(), GL_STATIC_DRAW);
  assert(!glGetError());


  if (!glowAlphaBufferId) {
    glGenBuffers(1, &glowAlphaBufferId);
    assert(!glGetError());
  }

  glBindBuffer(GL_ARRAY_BUFFER, glowAlphaBufferId);
  assert(!glGetError());

  glBufferData(GL_ARRAY_BUFFER, glowAlphaBufferData.size() * sizeof(float), &glowAlphaBufferData.front(), GL_STATIC_DRAW);
  assert(!glGetError());
}

void Figure::clearVertexBuffersData()
{
  figureVertexBufferData.clear();
  figureUVBufferData.clear();
  shadowVertexBufferData.clear();
  shadowUVBufferData.clear();
  glowVertexBufferData.clear();
  glowUVBufferData.clear();
  glowAlphaBufferData.clear();

  figureVertCount = 0;
  shadowVertCount = 0;
  glowVertCount = 0;
}

void Figure::addFigureVertex(float x, float y, float dx, float dy, float dv)
{
  const float du = 0.125f * float(color);

  figureVertexBufferData.push_back(x + dx);
  figureVertexBufferData.push_back(y + dy);
  figureUVBufferData.push_back(dx * 0.125f * 62.0f / 64.0f + du + 1.0f / 512.0f);
  figureUVBufferData.push_back(-dy * 0.25f * 62.0f / 64.0f / 2 + dv + 1.0f / 512.0f);

  figureVertCount++;
}

void Figure::addShadowVertex(float x, float y, float dx, float dy, float u, float v)
{
  shadowVertexBufferData.push_back(x + dx);
  shadowVertexBufferData.push_back(y + dy);
  shadowUVBufferData.push_back(u);
  shadowUVBufferData.push_back(v);

  shadowVertCount++;
}

void Figure::addGlowVertex(float x, float y, float dx, float dy, float alpha)
{
  glowVertexBufferData.push_back(x + dx);
  glowVertexBufferData.push_back(y + dy);
  glowUVBufferData.push_back(0.125f * float(color));
  glowUVBufferData.push_back(0.0625f);
  glowAlphaBufferData.push_back(alpha);

  glowVertCount++;
}

void Figure::rotate(int halfPiAngles)
{
}

void Figure::setScreenScale(const glm::vec2 & scale)
{
  screenScale = scale;

  figureProg.use();
  assert(!figureProg.isError());

  GLuint figureProjId = glGetUniformLocation(figureProg.getId(), "screenScale");
  assert(!glGetError());

  glUniform2fv(figureProjId, 1, &screenScale.x);
  assert(!glGetError());

  glowProg.use();
  assert(!glowProg.isError());

  GLuint glowProjId = glGetUniformLocation(glowProg.getId(), "screenScale");
  assert(!glGetError());

  glUniform2fv(glowProjId, 1, &screenScale.x);
  assert(!glGetError());
}

void Figure::drawFigure(float x, float y, float sqSize)
{
  Figure::figureProg.use();
  assert(!Figure::figureProg.isError());

  glBindTexture(GL_TEXTURE_2D, textureId);
  assert(!glGetError());


  GLuint posId = glGetUniformLocation(figureProg.getId(), "pos");
  assert(!glGetError());

  glUniform2f(posId, x, y);
  assert(!glGetError());

  GLuint sizeId = glGetUniformLocation(figureProg.getId(), "sqSize");
  assert(!glGetError());

  glUniform1f(sizeId, sqSize);
  assert(!glGetError());


  glEnableVertexAttribArray(0);
  assert(!glGetError());
  glEnableVertexAttribArray(1);
  assert(!glGetError());


  glBindBuffer(GL_ARRAY_BUFFER, figureVertexBufferId);
  assert(!glGetError());
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!glGetError());

  glBindBuffer(GL_ARRAY_BUFFER, figureUVBufferId);
  assert(!glGetError());
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!glGetError());

  glDrawArrays(GL_TRIANGLES, 0, figureVertCount);
  assert(!glGetError());


  glDisableVertexAttribArray(0);
  assert(!glGetError());
  glDisableVertexAttribArray(1);
  assert(!glGetError());
}

void Figure::drawShadow(float x, float y, float sqSize)
{
  Figure::figureProg.use();
  assert(!Figure::figureProg.isError());

  glBindTexture(GL_TEXTURE_2D, textureId);
  assert(!glGetError());


  GLuint posId = glGetUniformLocation(figureProg.getId(), "pos");
  assert(!glGetError());

  glUniform2f(posId, x, y);
  assert(!glGetError());

  GLuint sizeId = glGetUniformLocation(figureProg.getId(), "sqSize");
  assert(!glGetError());

  glUniform1f(sizeId, sqSize);
  assert(!glGetError());


  glEnableVertexAttribArray(0);
  assert(!glGetError());
  glEnableVertexAttribArray(1);
  assert(!glGetError());

  glBindBuffer(GL_ARRAY_BUFFER, shadowVertexBufferId);
  assert(!glGetError());
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!glGetError());

  glBindBuffer(GL_ARRAY_BUFFER, shadowUVBufferId);
  assert(!glGetError());
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!glGetError());

  glDrawArrays(GL_TRIANGLES, 0, shadowVertCount);
  assert(!glGetError());


  glDisableVertexAttribArray(0);
  assert(!glGetError());
  glDisableVertexAttribArray(1);
  assert(!glGetError());
}

void Figure::drawGlow(float x, float y, float sqSize)
{
  Figure::glowProg.use();
  assert(!Figure::glowProg.isError());

  glBindTexture(GL_TEXTURE_2D, textureId);
  assert(!glGetError());


  GLuint posId = glGetUniformLocation(glowProg.getId(), "pos");
  assert(!glGetError());

  glUniform2f(posId, x, y);
  assert(!glGetError());

  GLuint sizeId = glGetUniformLocation(glowProg.getId(), "sqSize");
  assert(!glGetError());

  glUniform1f(sizeId, sqSize);
  assert(!glGetError());


  glEnableVertexAttribArray(0);
  assert(!glGetError());
  glEnableVertexAttribArray(1);
  assert(!glGetError());
  glEnableVertexAttribArray(2);
  assert(!glGetError());

  glBindBuffer(GL_ARRAY_BUFFER, glowVertexBufferId);
  assert(!glGetError());
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!glGetError());

  glBindBuffer(GL_ARRAY_BUFFER, glowUVBufferId);
  assert(!glGetError());
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!glGetError());

  glBindBuffer(GL_ARRAY_BUFFER, glowAlphaBufferId);
  assert(!glGetError());
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!glGetError());


  glDrawArrays(GL_TRIANGLES, 0, glowVertCount);
  assert(!glGetError());


  glDisableVertexAttribArray(0);
  assert(!glGetError());
  glDisableVertexAttribArray(1);
  assert(!glGetError());
  glDisableVertexAttribArray(2);
  assert(!glGetError());
}
