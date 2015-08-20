#include "Figure.h"
#include "Crosy.h"

GLuint Figure::textureId = 0;
Program Figure::figureProg;
Shader Figure::figureVert(GL_VERTEX_SHADER);
Shader Figure::figureFrag(GL_FRAGMENT_SHADER);
//Program Figure::glowProg;
//Shader Figure::glowVert(GL_VERTEX_SHADER);
//Shader Figure::glowFrag(GL_FRAGMENT_SHADER);
glm::vec2 Figure::screenScale(1.0f);

// data - is a string of '0' and '1'
Figure::Figure(int dim, Color color, const char * data) :
  dim(dim),
  color(color),
  figureVertexBufferId(0),
  figureUVBufferId(0),
  borderMaskBufferId(0),
  vertCount(0)
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
  glDeleteBuffers(1, &borderMaskBufferId);
}

void Figure::init()
{
  figureVert.compileFromString(
    "#version 330 core\n"
    "layout(location = 0) in vec2 vertexPos;\n"
    "layout(location = 1) in vec2 vertexUV;\n"
    "uniform vec2 screenScale;\n"
    "uniform float sqSize;\n"
    "uniform vec2 pos;\n"
    "out vec2 uv;\n"

    "void main()\n"
    "{\n"
    "  gl_Position = vec4(screenScale * (vertexPos * sqSize + pos), 0, 1);\n"
    "  uv = vertexUV;\n"
    "}\n");
  assert(!figureVert.isError());

  figureFrag.compileFromString(
    "#version 330 core\n"
    "uniform sampler2D tex;\n"
    "in vec2 uv;\n"
    "out vec3 out_color;\n"

    "void main()\n"
    "{\n"
    "  out_color = texture(tex, uv).rgb;\n"
    "}\n");
  assert(!figureFrag.isError());

  figureProg.attachShader(figureVert);
  assert(!figureProg.isError());

  figureProg.attachShader(figureFrag);
  assert(!figureProg.isError());

  figureProg.link();
  assert(!figureProg.isError());

  figureProg.use();
  assert(!figureProg.isError());

  std::string texPath = Crosy::getExePath() + "\\textures\\blocks.jpg";
  textureId = SOIL_load_OGL_texture(texPath.c_str(), 0, 0, SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS);
  assert(textureId);
  assert(!glGetError());

  GLuint texId = glGetUniformLocation(figureProg.getId(), "tex");
  assert(!glGetError());

  glUniform1i(texId, 0);
  assert(!glGetError());

  //glowVert.compileFromString(
  //  "#version 330 core\n"
  //  "layout(location = 0) in vec2 vertexPos;\n"
  //  "uniform mat2 screenScale;\n"
  //  "uniform float sqSize;\n"
  //  "uniform vec2 pos;\n"

  //  "void main()\n"
  //  "{\n"
  //  "  gl_Position = vec4(screenScale * vertexPos, 1, 1);\n"
  //  "}\n");
  //assert(!glowVert.isError());

  //glowFrag.compileFromString(
  //  "#version 330 core\n"
  //  "out vec3 color;\n"

  //  "void main()\n"
  //  "{\n"
  //  "  color = vec3(1, 1, 1);\n"
  //  "}\n");
  //assert(!glowFrag.isError());

  //glowProg.attachShader(glowVert);
  //assert(!glowProg.isError());

  //glowProg.attachShader(glowFrag);
  //assert(!glowProg.isError());

  //glowProg.link();
  //assert(!glowProg.isError());
}

void Figure::buildMeshes()
{
  figureVertexBufferData.clear();
  figureUVBufferData.clear();
  borderMaskBufferData.clear();

  vertCount = 0;
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

      float dv;

      switch (bm & (bmLeft | bmLeftTop | bmTop))
      {
      case (bmLeft | bmLeftTop | bmTop) :
        dv = 0.25f; 
        break;
      case 0:
        dv = 0.0f; 
        break;
      case (bmTop) :
      case (bmTop | bmLeftTop) :
      case bmLeftTop:
        dv = 0.50f;
        break;
      case (bmLeft) :
      case (bmLeft | bmLeftTop) :
        dv = 0.75f; 
        break;
      default:
        dv = 0.0f;
      }

      addVertex(fx, fy, 0.0f, 0.0f, dv);
      addVertex(fx, fy, 0.5f, -0.5f, dv);
      addVertex(fx, fy, 0.0f, -0.5f, dv);

      switch (bm & (bmLeft | bmLeftTop | bmTop))
      {
      case (bmLeft | bmLeftTop | bmTop) :
        dv = 0.25f;
        break;
      case 0:
        dv = 0.0f;
        break;
      case (bmTop) :
      case (bmTop | bmLeftTop) :
        dv = 0.5f;
        break;
      case (bmLeft) :
      case (bmLeft | bmLeftTop) :
      case bmLeftTop:
        dv = 0.75f;
        break;
      default:
        dv = 0.0f;
      }

      addVertex(fx, fy, 0.0f, 0.0f, dv);
      addVertex(fx, fy, 0.5f, 0.0f, dv);
      addVertex(fx, fy, 0.5f, -0.5f, dv);

      switch (bm & (bmTop | bmTopRight | bmRight))
      {
      case (bmTop | bmTopRight | bmRight) :
        dv = 0.25f;
        break;
      case 0:
        dv = 0.0f;
        break;
      case (bmTop) :
      case (bmTop | bmTopRight) :
        dv = 0.5f;
        break;
      case (bmRight) :
      case (bmRight | bmTopRight) :
      case bmTopRight:
        dv = 0.75f;
        break;
      default:
        dv = 0.0f;
      }

      addVertex(fx, fy, 0.5f, 0.0f, dv);
      addVertex(fx, fy, 1.0f, 0.0f, dv);
      addVertex(fx, fy, 0.5f, -0.5f, dv);

      switch (bm & (bmRight | bmTopRight | bmTop))
      {
      case (bmRight | bmTopRight | bmTop) :
        dv = 0.25f;
        break;
      case 0:
        dv = 0.0f;
        break;
      case (bmTop) :
      case (bmTop | bmTopRight) :
      case bmTopRight:
        dv = 0.50f;
        break;
      case (bmRight) :
      case (bmRight | bmTopRight) :
        dv = 0.75f;
        break;
      default:
        dv = 0.0f;
      }

      addVertex(fx, fy, 1.0f, 0.0f, dv);
      addVertex(fx, fy, 1.0f, -0.5f, dv);
      addVertex(fx, fy, 0.5f, -0.5f, dv);

      switch (bm & (bmRight | bmRightBottom | bmBottom))
      {
      case (bmRight | bmRightBottom | bmBottom) :
        dv = 0.25f;
        break;
      case 0:
        dv = 0.0f;
        break;
      case (bmBottom) :
      case (bmBottom | bmRightBottom) :
      case bmRightBottom:
        dv = 0.50f;
        break;
      case (bmRight) :
      case (bmRight | bmRightBottom) :
        dv = 0.75f;
        break;
      default:
        dv = 0.0f;
      }

      addVertex(fx, fy, 0.5f, -0.5f, dv);
      addVertex(fx, fy, 1.0f, -0.5f, dv);
      addVertex(fx, fy, 1.0f, -1.0f, dv);

      switch (bm & (bmRight | bmRightBottom | bmBottom))
      {
      case (bmRight | bmRightBottom | bmBottom) :
        dv = 0.25f;
        break;
      case 0:
        dv = 0.0f;
        break;
      case (bmBottom) :
      case (bmBottom | bmRightBottom) :
        dv = 0.5f;
        break;
      case (bmRight) :
      case (bmRight | bmRightBottom) :
      case bmRightBottom:
        dv = 0.75f;
        break;
      default:
        dv = 0.0f;
      }

      addVertex(fx, fy, 0.5f, -0.5f, dv);
      addVertex(fx, fy, 1.0f, -1.0f, dv);
      addVertex(fx, fy, 0.5f, -1.0f, dv);

      switch (bm & (bmBottom | bmBottomLeft | bmLeft))
      {
      case (bmBottom | bmBottomLeft | bmLeft) :
        dv = 0.25f;
        break;
      case 0:
        dv = 0.0f;
        break;
      case (bmBottom) :
      case (bmBottom | bmBottomLeft) :
                                dv = 0.5f;
        break;
      case (bmLeft) :
      case (bmLeft | bmBottomLeft) :
      case bmBottomLeft:
        dv = 0.75f;
        break;
      default:
        dv = 0.0f;
      }

      addVertex(fx, fy, 0.5f, -0.5f, dv);
      addVertex(fx, fy, 0.5f, -1.0f, dv);
      addVertex(fx, fy, 0.0f, -1.0f, dv);

      switch (bm & (bmLeft | bmBottomLeft | bmBottom))
      {
      case (bmLeft | bmBottomLeft | bmBottom) :
        dv = 0.25f;
        break;
      case 0:
        dv = 0.0f;
        break;
      case (bmBottom) :
      case (bmBottom | bmBottomLeft) :
      case bmBottomLeft:
        dv = 0.50f;
        break;
      case (bmLeft) :
      case (bmLeft | bmBottomLeft) :
                                  dv = 0.75f;
        break;
      default:
        dv = 0.0f;
      }

      addVertex(fx, fy, 0.5f, -0.5f, dv);
      addVertex(fx, fy, 0.0f, -1.0f, dv);
      addVertex(fx, fy, 0.0f, -0.5f, dv);
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

}

void Figure::addVertex(float x, float y, float dx, float dy, float dv)
{
  const float du = 0.125f * float(color);

  figureVertexBufferData.push_back(x + dx);
  figureVertexBufferData.push_back(y + dy);
  figureUVBufferData.push_back(dx * 0.125f * 62.0f / 64.0f + du + 1.0f / 512.0f);
  figureUVBufferData.push_back(-dy * 0.25f * 62.0f / 64.0f + dv + 1.0f / 256.0f);

  vertCount ++;
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
}

void Figure::draw(float x, float y, float sqSize)
{
  Figure::figureProg.use();
  assert(!Figure::figureProg.isError());

  glBindTexture(GL_TEXTURE_2D, textureId);
  assert(!glGetError());


  glBindBuffer(GL_ARRAY_BUFFER, figureVertexBufferId);
  assert(!glGetError());

  glEnableVertexAttribArray(0);
  assert(!glGetError());

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!glGetError());


  glBindBuffer(GL_ARRAY_BUFFER, figureUVBufferId);
  assert(!glGetError());

  glEnableVertexAttribArray(1);
  assert(!glGetError());

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!glGetError());


  GLuint posId = glGetUniformLocation(figureProg.getId(), "pos");
  assert(!glGetError());

  glUniform2f(posId, x, y);
  assert(!glGetError());

  GLuint texId = glGetUniformLocation(figureProg.getId(), "color");
  assert(!glGetError());

  glUniform1i(texId, color);
  assert(!glGetError());

  GLuint sizeId = glGetUniformLocation(figureProg.getId(), "sqSize");
  assert(!glGetError());

  glUniform1f(sizeId, sqSize);
  assert(!glGetError());


  glDrawArrays(GL_TRIANGLES, 0, vertCount);
  assert(!glGetError());


  glDisableVertexAttribArray(0);
  assert(!glGetError());

  glDisableVertexAttribArray(1);
  assert(!glGetError());

  glDisableVertexAttribArray(2);
  assert(!glGetError());
}
