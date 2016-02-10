#include "static_headers.h"

#include "OpenGLRender.h"
#include "Globals.h"
#include "DropSparkle.h"
#include "DropTrail.h"
#include "Crosy.h"
#include "Time.h"
#include "Layout.h"
#include "Palette.h"

OpenGLRender::OpenGLRender() :
  commonVert(GL_VERTEX_SHADER),
  commonFrag(GL_FRAGMENT_SHADER),
  fontVert(GL_VERTEX_SHADER),
  fontFrag(GL_FRAGMENT_SHADER),
  edgeBlurWidth(0.005f),
  showWireframe(false)
{
  for (int ind = (int)FIRST_TEX_INDEX; ind < TEX_INDEX_COUNT; ind++)
  {
    texPos[ind].x = 0.0625f + 0.25f * (ind % 4);
    texPos[ind].y = 0.0625f + 0.25f * (ind / 4);
  }

  bkVertexBuffer.reserve(16384);
  atlasVertexBuffer.reserve(131072);
  textVertexBuffer.reserve(4096);
}


void OpenGLRender::init(int width, int height)
{
  glGenVertexArrays(1, &vaoId);
  assert(!checkGlErrors());
  glBindVertexArray(vaoId);
  assert(!checkGlErrors());
  glGenBuffers(1, &bkVertexBufferId);
  assert(!checkGlErrors());
  glGenBuffers(1, &atlasVertexBufferId);
  assert(!checkGlErrors());
  glGenBuffers(1, &textVertexBufferId);
  assert(!checkGlErrors());

  commonVert.init();
  commonVert.compileFromString(
    "#version 120\n"
    "attribute vec2 vertexPos;"
    "attribute vec2 vertexUV;"
    "attribute vec4 vertexRGBA;"
    "varying vec2 uv;"
    "varying vec4 color;"
    "varying vec2 pixPos;"

    "void main()"
    "{"
    "  gl_Position = vec4(vertexPos.x * 2.0 - 1.0, 1.0 - vertexPos.y * 2.0, 0, 1);"
    "  uv = vertexUV;"
    "  color = vertexRGBA;"
    "  pixPos = vertexPos;"
    "}");

  commonFrag.init();
  commonFrag.compileFromString(
    "#version 120\n"
    "uniform sampler2D tex;"
    "varying vec2 uv;"
    "varying vec4 color;"
    "varying vec2 pixPos;"

    "float rand(vec2 co){"
    "  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);"
    "}"

    "void main()"
    "{"
    "  vec3 texcol = texture2D(tex, uv).rgb;"
    "  float alpha = (1.0 - texcol.b) * color.a;"
    "  vec3 rgb = mix(texcol.r * color.rgb, texcol.rrr, texcol.g);"
    "  gl_FragColor = vec4(rgb * (1.0 + (rand(pixPos) - 0.5) / 30.0 * alpha), alpha);"
    "}");

  commonProg.init();
  commonProg.attachShader(commonVert);
  commonProg.attachShader(commonFrag);
  commonProg.bindAttribLocation(0, "vertexPos");
  commonProg.bindAttribLocation(1, "vertexUV");
  commonProg.bindAttribLocation(2, "vertexRGBA");
  commonProg.link();
  commonProg.use();
  commonProg.setUniform("tex", 0);

  fontVert.init();
  fontVert.compileFromString(
    "#version 120\n"
    "attribute vec2 vertexPos;"
    "attribute vec2 vertexUV;"
    "attribute vec4 vertexRGBA;"
    "attribute float vertexBlur;"
    "varying vec2 uv;"
    "varying float threshold;"
    "varying vec4 color;"

    "void main()"
    "{"
    "  gl_Position = vec4(vertexPos.x * 2.0 - 1.0, 1.0 - vertexPos.y * 2.0, 0, 1);"
    "  uv = vertexUV;"
    "  threshold = vertexBlur;"
    "  color = vertexRGBA;"
    "}");

  fontFrag.init();
  fontFrag.compileFromString(
    "#version 120\n"
    "uniform sampler2D tex;"
    "varying vec2 uv;"
    "varying float threshold;"
    "varying vec4 color;"

    "void main()"
    "{"
    "  float alpha = smoothstep(0.5 - threshold, 0.5 + threshold, texture2D(tex, uv).a);"
    "  gl_FragColor = color * alpha;"
    "}");

  fontProg.init();
  fontProg.attachShader(fontVert);
  fontProg.attachShader(fontFrag);
  fontProg.bindAttribLocation(0, "vertexPos");
  fontProg.bindAttribLocation(1, "vertexUV");
  fontProg.bindAttribLocation(2, "vertexRGBA");
  fontProg.bindAttribLocation(3, "vertexBlur");
  fontProg.link();
  fontProg.use();
  fontProg.setUniform("tex", 0);

  int imageWidth, imageHeight, channels;
  std::string bkTextureFileName = Crosy::getExePath() + "\\textures\\BackgroundTile.png";

  if (unsigned char * bkTextureImage = stbi_load(bkTextureFileName.c_str(),
      &imageWidth, &imageHeight, &channels, 4))
  {
    glGenTextures(1, &bkTextureId);
    assert(!checkGlErrors());
    glBindTexture(GL_TEXTURE_2D, bkTextureId);
    assert(!checkGlErrors());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 bkTextureImage);
    assert(!checkGlErrors());
    free(bkTextureImage);
    glGenerateMipmap(GL_TEXTURE_2D);
    assert(!checkGlErrors());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert(!checkGlErrors());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert(!checkGlErrors());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1);
    assert(!checkGlErrors());
  }
  else
    assert(0);

  std::string mainAtlasFileName = Crosy::getExePath() + "\\textures\\MainAtlas.png";

  if (unsigned char * mainAtlasImage = stbi_load(mainAtlasFileName.c_str(),
      &imageWidth, &imageHeight, &channels, 4))
  {
    glGenTextures(1, &atlasTextureId);
    assert(!checkGlErrors());
    glBindTexture(GL_TEXTURE_2D, atlasTextureId);
    assert(!checkGlErrors());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 mainAtlasImage);
    assert(!checkGlErrors());
    free(mainAtlasImage);
    glGenerateMipmap(GL_TEXTURE_2D);
    assert(!checkGlErrors());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert(!checkGlErrors());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert(!checkGlErrors());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1);
    assert(!checkGlErrors());
  }
  else
    assert(0);

  std::string fontTextureFileName = Crosy::getExePath() + "\\fonts\\MontserratTexture.png";
  std::string fontMetricsFileName = Crosy::getExePath() + "\\fonts\\MontserratMetrics.json";
  font.load(fontMetricsFileName.c_str());

  if (unsigned char * fontTextureImage = stbi_load(fontTextureFileName.c_str(),
      &imageWidth, &imageHeight, &channels, 1))
  {
    glGenTextures(1, &fontTextureId);
    assert(!checkGlErrors());
    glBindTexture(GL_TEXTURE_2D, fontTextureId);
    assert(!checkGlErrors());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, imageWidth, imageHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE,
                 fontTextureImage);
    assert(!checkGlErrors());
    free(fontTextureImage);
    glGenerateMipmap(GL_TEXTURE_2D);
    assert(!checkGlErrors());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert(!checkGlErrors());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert(!checkGlErrors());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1);
    assert(!checkGlErrors());
  }
  else
    assert(0);

  glDisable(GL_CULL_FACE);
  assert(!checkGlErrors());
  glEnable(GL_DEPTH_TEST);
  assert(!checkGlErrors());
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  assert(!checkGlErrors());
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  assert(!checkGlErrors());

  resize(width, height);
}


void OpenGLRender::quit()
{
  commonProg.quit();
  commonVert.quit();
  commonFrag.quit();
  fontProg.quit();
  fontVert.quit();
  fontFrag.quit();
}


void OpenGLRender::resize(int width, int height)
{
  this->width = width;
  this->height = height;

  const float gameAspect = Layout::backgroundWidth / Layout::backgroundHeight;

  if (!height || float(width) / height > gameAspect)
  {
    pxSize = 1.0f / height;
    glViewport((width - height) / 2, 0, height, height);
    assert(!checkGlErrors());
  }
  else
  {
    pxSize = gameAspect / width;
    const int x = int(width * (1.0f - 1.0f / gameAspect)) / 2;
    const int y = (height - int(width / gameAspect)) / 2;
    glViewport(x, y, int(width / gameAspect), int(width / gameAspect));
    assert(!checkGlErrors());
  }

  // filling all screen depth buffer with -1.0f value and then rendering 
  // background with z = 0.0f to avoid firther rendering out of background area
  glClearDepth(-1.0f);
  assert(!checkGlErrors());
  glClear(GL_DEPTH_BUFFER_BIT);
  assert(!checkGlErrors());
  glDepthFunc(GL_ALWAYS);
  assert(!checkGlErrors());

  clearVertices();
  buildBackground();
  drawMesh();

  glDepthFunc(GL_LEQUAL);
  assert(!checkGlErrors());
}


void OpenGLRender::update()
{
#ifdef _DEBUG
  if (showWireframe)
  {
    glDisable(GL_BLEND);
    assert(!checkGlErrors());
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    assert(!checkGlErrors());
    glLogicOp(GL_SET);
    assert(!checkGlErrors());
    glEnable(GL_COLOR_LOGIC_OP);
    assert(!checkGlErrors());
  }
  else
#endif _DEBUG
  {
    glEnable(GL_BLEND);
    assert(!checkGlErrors());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(!checkGlErrors());
    glDisable(GL_COLOR_LOGIC_OP);
    assert(!checkGlErrors());
  }

  glClear(GL_COLOR_BUFFER_BIT);
  assert(!checkGlErrors());

  updateGameLayer();
  updateSettingsLayer();
  updateLeaderboardLayer();
  updateMenuLayer();
}


void OpenGLRender::drawMesh()
{
  if (!bkVertexBuffer.empty())
  {
    glEnableVertexAttribArray(0);
    assert(!checkGlErrors());
    glEnableVertexAttribArray(1);
    assert(!checkGlErrors());
    glEnableVertexAttribArray(2);
    assert(!checkGlErrors());

    commonProg.use();

    glBindTexture(GL_TEXTURE_2D, bkTextureId);
    assert(!checkGlErrors());
    glBindBuffer(GL_ARRAY_BUFFER, bkVertexBufferId);
    assert(!checkGlErrors());
    glBufferData(GL_ARRAY_BUFFER, bkVertexBuffer.size() * sizeof(Vertex), bkVertexBuffer.data(), GL_STATIC_DRAW);
    assert(!checkGlErrors());
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, xy));
    assert(!checkGlErrors());
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    assert(!checkGlErrors());
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, rgba));
    assert(!checkGlErrors());

    glDrawArrays(GL_TRIANGLES, 0, (int)bkVertexBuffer.size());
    assert(!checkGlErrors());

    glDisableVertexAttribArray(0);
    assert(!checkGlErrors());
    glDisableVertexAttribArray(1);
    assert(!checkGlErrors());
    glDisableVertexAttribArray(2);
    assert(!checkGlErrors());
  }

  if (!atlasVertexBuffer.empty())
  {
    glEnableVertexAttribArray(0);
    assert(!checkGlErrors());
    glEnableVertexAttribArray(1);
    assert(!checkGlErrors());
    glEnableVertexAttribArray(2);
    assert(!checkGlErrors());

    commonProg.use();

    glBindTexture(GL_TEXTURE_2D, atlasTextureId);
    assert(!checkGlErrors());
    glBindBuffer(GL_ARRAY_BUFFER, atlasVertexBufferId);
    assert(!checkGlErrors());
    glBufferData(GL_ARRAY_BUFFER, atlasVertexBuffer.size() * sizeof(Vertex), atlasVertexBuffer.data(), GL_STATIC_DRAW);
    assert(!checkGlErrors());
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, xy));
    assert(!checkGlErrors());
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    assert(!checkGlErrors());
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, rgba));
    assert(!checkGlErrors());

    glDrawArrays(GL_TRIANGLES, 0, (int)atlasVertexBuffer.size());
    assert(!checkGlErrors());

    glDisableVertexAttribArray(0);
    assert(!checkGlErrors());
    glDisableVertexAttribArray(1);
    assert(!checkGlErrors());
    glDisableVertexAttribArray(2);
    assert(!checkGlErrors());
  }

  if (!textVertexBuffer.empty())
  {
    glEnableVertexAttribArray(0);
    assert(!checkGlErrors());
    glEnableVertexAttribArray(1);
    assert(!checkGlErrors());
    glEnableVertexAttribArray(2);
    assert(!checkGlErrors());
    glEnableVertexAttribArray(3);
    assert(!checkGlErrors());

    fontProg.use();

    glBindTexture(GL_TEXTURE_2D, fontTextureId);
    assert(!checkGlErrors());
    glBindBuffer(GL_ARRAY_BUFFER, textVertexBufferId);
    assert(!checkGlErrors());
    glBufferData(GL_ARRAY_BUFFER, textVertexBuffer.size() * sizeof(TextVertex), textVertexBuffer.data(), GL_STATIC_DRAW);
    assert(!checkGlErrors());
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, xy));
    assert(!checkGlErrors());
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, uv));
    assert(!checkGlErrors());
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, rgba));
    assert(!checkGlErrors());
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, falloff));
    assert(!checkGlErrors());

    glDrawArrays(GL_TRIANGLES, 0, (int)textVertexBuffer.size());
    assert(!checkGlErrors());

    glDisableVertexAttribArray(0);
    assert(!checkGlErrors());
    glDisableVertexAttribArray(1);
    assert(!checkGlErrors());
    glDisableVertexAttribArray(2);
    assert(!checkGlErrors());
    glDisableVertexAttribArray(3);
    assert(!checkGlErrors());
  }
}


void OpenGLRender::addBkVertex(const glm::vec2 & xy, const glm::vec2 & uv, 
                               const glm::vec3 & color, float alpha)
{
  Vertex vertex;
  vertex.xy = xy;
  vertex.uv = uv;
  vertex.rgba = glm::vec4(color, alpha);
  bkVertexBuffer.push_back(vertex);
}


void OpenGLRender::addAtlasVertex(const glm::vec2 & xy, const glm::vec2 & uv, int texIndex, 
                                  const glm::vec3 & color, float alpha)
{
  Vertex vertex;
  vertex.xy = xy;
  vertex.uv = texPos[texIndex] + 0.125f * uv;
  vertex.rgba = glm::vec4(color, alpha);
  atlasVertexBuffer.push_back(vertex);
}


void OpenGLRender::addTextVertex(const glm::vec2 & xy, const glm::vec2 & uv, float falloffSize, 
                                 const glm::vec3 & color, float alpha)
{
  TextVertex vertex;
  vertex.xy = xy;
  vertex.uv = uv;
  vertex.rgba = glm::vec4(color, alpha);
  vertex.falloff = falloffSize;
  textVertexBuffer.push_back(vertex);
}


void OpenGLRender::clearVertices()
{
  bkVertexBuffer.clear();
  atlasVertexBuffer.clear();
  textVertexBuffer.clear();
}


void OpenGLRender::buildRect(float left, float top, float width, float height, 
                             const glm::vec3 & color, float alpha)
{
  const glm::vec2 verts0(left, top);
  const glm::vec2 verts1(left + width, top);
  const glm::vec2 verts2(left, top + height);
  const glm::vec2 verts3(left + width, top + height);
  const glm::vec2 uv(0.5f);
  // TODO : overload addAtlasVertex to give x, y, u, v arguments to reduce glm::vec2 constructions
  addAtlasVertex(verts0, uv, tiEmpty, color, alpha);
  addAtlasVertex(verts1, uv, tiEmpty, color, alpha);
  addAtlasVertex(verts2, uv, tiEmpty, color, alpha);
  addAtlasVertex(verts1, uv, tiEmpty, color, alpha);
  addAtlasVertex(verts2, uv, tiEmpty, color, alpha);
  addAtlasVertex(verts3, uv, tiEmpty, color, alpha);
}


void OpenGLRender::buildSmoothRect(float left, float top, float width, float height, 
                                   float blur, const glm::vec3 & color, float alpha)
{
  blur = glm::min(glm::min(blur, 0.5f * width), 0.5f * height);
  const float halfBlur = 0.5f * blur;
  buildRect(left + halfBlur, top + halfBlur, width - blur, height - blur, color, alpha);
  buildFrameRect(left, top, width, height, blur, color, alpha);
}


void OpenGLRender::buildTexturedRect(float left, float top, float width, float height, int texIndex, 
                                     const glm::vec3 & color, float alpha)
{
  const glm::vec2 verts0(left, top);
  const glm::vec2 verts1(left + width, top);
  const glm::vec2 verts2(left, top + height);
  const glm::vec2 verts3(left + width, top + height);
  const glm::vec2 uv0(0.0f, 0.0f);
  const glm::vec2 uv1(1.0f, 0.0f);
  const glm::vec2 uv2(0.0f, 1.0f);
  const glm::vec2 uv3(1.0f, 1.0f);

  addAtlasVertex(verts0, uv0, texIndex, color, alpha);
  addAtlasVertex(verts1, uv1, texIndex, color, alpha);
  addAtlasVertex(verts2, uv2, texIndex, color, alpha);
  addAtlasVertex(verts1, uv1, texIndex, color, alpha);
  addAtlasVertex(verts2, uv2, texIndex, color, alpha);
  addAtlasVertex(verts3, uv3, texIndex, color, alpha);
}


void OpenGLRender::buildVertGradientRect(float left, float top, float width, float height, 
                                         const glm::vec3 & topColor, float topAlpha, 
                                         const glm::vec3 & bottomColor, float bottomAlpha)
{
  glm::vec2 verts0(left, top);
  glm::vec2 verts1(left + width, top);
  glm::vec2 verts2(left, top + height);
  glm::vec2 verts3(left + width, top + height);
  const glm::vec2 uv(0.5f);

  addAtlasVertex(verts0, uv, tiEmpty, topColor, topAlpha);
  addAtlasVertex(verts1, uv, tiEmpty, topColor, topAlpha);
  addAtlasVertex(verts2, uv, tiEmpty, bottomColor, bottomAlpha);
  addAtlasVertex(verts1, uv, tiEmpty, topColor, topAlpha);
  addAtlasVertex(verts2, uv, tiEmpty, bottomColor, bottomAlpha);
  addAtlasVertex(verts3, uv, tiEmpty, bottomColor, bottomAlpha);
}


void OpenGLRender::buildLine(float x0, float y0, float x1, float y1, float width, 
                             const glm::vec3 & color, float alpha)
{
  const float W_2 = 0.5f * width;
  glm::vec2 v0(x0, y0);
  glm::vec2 v1(x1, y1);
  glm::vec2 v10 = v1 - v0;
  glm::vec2 v10_normalized = glm::normalize(v10);
  glm::vec2 v10_normal = glm::vec2(-v10_normalized.y, v10_normalized.x);

  glm::vec2 verts[8] =
  {
    v0 + W_2 * v10_normal - W_2 * v10_normalized,
    v0 - W_2 * v10_normal - W_2 * v10_normalized,
    v0 + W_2 * v10_normal,
    v0 - W_2 * v10_normal,

    v1 + W_2 * v10_normal,
    v1 - W_2 * v10_normal,
    v1 + W_2 * v10_normal + W_2 * v10_normalized,
    v1 - W_2 * v10_normal + W_2 * v10_normalized,
  };

  glm::vec2 uv[8] =
  {
    { 0.0f, 0.0f },
    { 0.0f, 1.0f },
    { 0.5f, 0.0f },
    { 0.5f, 1.0f },
    { 0.5f, 0.0f },
    { 0.5f, 1.0f },
    { 0.0f, 0.0f },
    { 0.0f, 1.0f },
  };

  for (int i = 0; i < 6; i += 2)
  {
    addAtlasVertex(verts[i + 0], uv[i + 0], tiLine, color, alpha);
    addAtlasVertex(verts[i + 1], uv[i + 1], tiLine, color, alpha);
    addAtlasVertex(verts[i + 2], uv[i + 2], tiLine, color, alpha);

    addAtlasVertex(verts[i + 1], uv[i + 1], tiLine, color, alpha);
    addAtlasVertex(verts[i + 2], uv[i + 2], tiLine, color, alpha);
    addAtlasVertex(verts[i + 3], uv[i + 3], tiLine, color, alpha);
  }
}


void OpenGLRender::buildFrameRect(float left, float top, float width, float height, float borderWidth, 
                                  const glm::vec3 & borderColor, float borderAlpha)
{
  glm::vec2 verts[8] =
  {
    { left, top },
    { left + borderWidth, top + borderWidth },
    { left + width, top },
    { left + width - borderWidth, top + borderWidth },
    { left + width, top + height },
    { left + width - borderWidth, top + height - borderWidth },
    { left, top + height },
    { left + borderWidth, top + height - borderWidth },

  };

  glm::vec2 uv[8] =
  {
    { 0.5f, 0.0f },
    { 0.5f, 1.0f },
    { 0.5f, 0.0f },
    { 0.5f, 1.0f },
    { 0.5f, 0.0f },
    { 0.5f, 1.0f },
    { 0.5f, 0.0f },
    { 0.5f, 1.0f },
  };

  for (int i = 0; i < 8; i += 2)
  {
    addAtlasVertex(verts[i + 0], uv[i + 0], tiLine, borderColor, borderAlpha);
    addAtlasVertex(verts[i + 1], uv[i + 1], tiLine, borderColor, borderAlpha);
    addAtlasVertex(verts[(i + 2) & 7], uv[(i + 2) & 7], tiLine, borderColor, borderAlpha);

    addAtlasVertex(verts[i + 1], uv[i + 1], tiLine, borderColor, borderAlpha);
    addAtlasVertex(verts[(i + 2) & 7], uv[(i + 2) & 7], tiLine, borderColor, borderAlpha);
    addAtlasVertex(verts[(i + 3) & 7], uv[(i + 3) & 7], tiLine, borderColor, borderAlpha);
  }
}


void OpenGLRender::buildProgressBar(float left, float top, float width, float height, 
                                    const glm::vec3 & bkColor, const glm::vec3 & borderColor, 
                                    const glm::vec3 & barColor, float alpha, float progress)
{
  const float borderWidth = Layout::settingsProgressBarBorder;
  const float gapWidth = 0.5f * Layout::settingsProgressBarInnerGap;

  buildRect(left + 0.5f * borderWidth, top + 0.5f * borderWidth,
            width - borderWidth, height - borderWidth, bkColor, alpha);

  if (progress > 0.0f)
  {
    buildSmoothRect(left + 0.5f * borderWidth, top + 0.5f * borderWidth,
                    (width - borderWidth) * progress, height - borderWidth, edgeBlurWidth, barColor, alpha);
  }

  buildFrameRect(left, top, width, height, borderWidth, borderColor, alpha);
}


void OpenGLRender::buildBackground()
{
  // base game background
  glm::vec2 origin(Layout::backgroundLeft, Layout::backgroundTop);

  const int xTess = 6;
  const int yTess = 6;

  for (int y = 0; y < yTess; y++)
  for (int x = 0; x < xTess; x++)
  {
    glm::vec2 verts[4] =
    {
      { Layout::backgroundWidth * x / xTess,        Layout::backgroundHeight * y / yTess },
      { Layout::backgroundWidth * x / xTess,        Layout::backgroundHeight * (y + 1) / yTess },
      { Layout::backgroundWidth * (x + 1) / xTess,  Layout::backgroundHeight * y / yTess },
      { Layout::backgroundWidth * (x + 1) / xTess,  Layout::backgroundHeight * (y + 1) / yTess },
    };

    const float tiledU = Layout::backgroundWidth / Layout::gameBkTileWidth;
    const float tiledV = Layout::backgroundHeight / Layout::gameBkTileHeight;

    glm::vec2 uv[4] =
    {
      { tiledU * x / xTess,        tiledV * y / yTess },
      { tiledU * x / xTess,        tiledV * (y + 1) / yTess },
      { tiledU * (x + 1) / xTess,  tiledV * y / yTess },
      { tiledU * (x + 1) / xTess,  tiledV * (y + 1) / yTess },
    };

    float fx0 = float(abs(xTess - 2 * x)) / xTess;
    float fy0 = float(y) / yTess;
    float fx1 = float(abs(xTess - 2 * (x + 1))) / xTess;
    float fy1 = float(y + 1) / yTess;

    float bright[4] =
    {
      1.0f - sqrtf(fx0 * fx0 + fy0 * fy0) / M_SQRT2,
      1.0f - sqrtf(fx0 * fx0 + fy1 * fy1) / M_SQRT2,
      1.0f - sqrtf(fx1 * fx1 + fy0 * fy0) / M_SQRT2,
      1.0f - sqrtf(fx1 * fx1 + fy1 * fy1) / M_SQRT2,
    };

    const glm::vec3 & innerColor = Palette::gameBackgroundInner;
    const glm::vec3 & outerColor = Palette::gameBackgroundOuter;

    glm::vec3 col[4] =
    {
      outerColor + (innerColor - outerColor) * bright[0],
      outerColor + (innerColor - outerColor) * bright[1],
      outerColor + (innerColor - outerColor) * bright[2],
      outerColor + (innerColor - outerColor) * bright[3],
    };

    addBkVertex(origin + verts[0], uv[0], col[0], 1.0f);
    addBkVertex(origin + verts[1], uv[1], col[1], 1.0f);
    addBkVertex(origin + verts[2], uv[2], col[2], 1.0f);
    addBkVertex(origin + verts[1], uv[1], col[1], 1.0f);
    addBkVertex(origin + verts[2], uv[2], col[2], 1.0f);
    addBkVertex(origin + verts[3], uv[3], col[3], 1.0f);
  }

  // field background
  if (LayoutObject * fieldLayout = Layout::screen.getChildRecursive(loField))
  {
    origin.x = fieldLayout->getGlobalLeft();
    origin.y = fieldLayout->getGlobalTop();
    const float fieldWidth = fieldLayout->width;
    const float fieldHeight = fieldLayout->height;

    for (int y = 0, yTess = 6; y < yTess; y++)
    for (int x = 0, xTess = 6; x < xTess; x++)
    {
      glm::vec2 verts[4] =
      {
        { fieldWidth * x / xTess,       fieldHeight * y / yTess },
        { fieldWidth * x / xTess,       fieldHeight * (y + 1) / yTess },
        { fieldWidth * (x + 1) / xTess, fieldHeight * y / yTess },
        { fieldWidth * (x + 1) / xTess, fieldHeight * (y + 1) / yTess },
      };

      const float fx0 = float(abs(xTess - 2 * x)) / xTess;
      const float fy0 = float(abs(yTess / 4 - y)) / yTess;
      const float fx1 = float(abs(xTess - 2 * (x + 1))) / xTess;
      const float fy1 = float(abs(yTess / 4 - (y + 1))) / yTess;

      float bright[4] =
      {
        sqrtf(fx0 * fx0 + fy0 * fy0) / M_SQRT2,
        sqrtf(fx0 * fx0 + fy1 * fy1) / M_SQRT2,
        sqrtf(fx1 * fx1 + fy0 * fy0) / M_SQRT2,
        sqrtf(fx1 * fx1 + fy1 * fy1) / M_SQRT2,
      };

      double a = 0.0f;
      static double freq = (double)Crosy::getPerformanceFrequency();

      if (freq)
        a = double(Crosy::getPerformanceCounter()) / freq / 300.0f;

      const float rMin = Palette::fieldBackgroundMin.r;
      const float gMin = Palette::fieldBackgroundMin.g;
      const float bMin = Palette::fieldBackgroundMin.b;
      const float rMax = Palette::fieldBackgroundMax.r;
      const float gMax = Palette::fieldBackgroundMax.g;
      const float bMax = Palette::fieldBackgroundMax.b;
      const float darkMul = Palette::fieldBackgroundInnerBright;
      const float darkDiff = (float)M_PI / 20.0f;
      const float gDiff = (float)M_PI / 3.0f;
      const float bDiff = 2.0f * (float)M_PI / 3.0f;

      glm::vec3 outerColor(rMin + (rMax - rMin) * abs(sin(a)),
        gMin + (gMax - gMin) * abs(sin(a + gDiff)),
        bMin + (bMax - bMin) * abs(sin(a + bDiff)));

      glm::vec3 innerColor(darkMul * (rMin + (rMax - rMin) * abs(sin(a + darkDiff))),
        darkMul * (gMin + (gMax - gMin) * abs(sin(a + gDiff + darkDiff))),
        darkMul * (bMin + (bMax - bMin) * abs(sin(a + bDiff + darkDiff))));

      glm::vec3 col[4] =
      {
        innerColor + (outerColor - innerColor) * bright[0],
        innerColor + (outerColor - innerColor) * bright[1],
        innerColor + (outerColor - innerColor) * bright[2],
        innerColor + (outerColor - innerColor) * bright[3],
      };

      glm::vec2 uv(0.5f, 0.5f);

      addAtlasVertex(origin + verts[0], uv, tiEmpty, col[0], 1.0f);
      addAtlasVertex(origin + verts[1], uv, tiEmpty, col[1], 1.0f);
      addAtlasVertex(origin + verts[2], uv, tiEmpty, col[2], 1.0f);
      addAtlasVertex(origin + verts[1], uv, tiEmpty, col[1], 1.0f);
      addAtlasVertex(origin + verts[2], uv, tiEmpty, col[2], 1.0f);
      addAtlasVertex(origin + verts[3], uv, tiEmpty, col[3], 1.0f);
    }
  }

  // score caption
  if (LayoutObject * scoreBarCaptionLayout = Layout::screen.getChildRecursive(loScoreBarCaption))
  {
    const float left = scoreBarCaptionLayout->getGlobalLeft();
    const float top = scoreBarCaptionLayout->getGlobalTop();
    const float width = scoreBarCaptionLayout->width;
    const float height = scoreBarCaptionLayout->height;
    buildRect(left, top, width, height, Palette::scoreBarBackground, Palette::scoreBarBackgroundAlpha);
    buildTextMesh(left, top, width, height, "SCORE", height,
                  Palette::scoreBarText, 1.0f, 0.0f, haCenter, vaCenter);
  }

  // score value
  if (LayoutObject * scoreBarValueLayout = Layout::screen.getChildRecursive(loScoreBarValue))
  {
    const float left = scoreBarValueLayout->getGlobalLeft();
    const float top = scoreBarValueLayout->getGlobalTop();
    const float width = scoreBarValueLayout->width;
    const float height = scoreBarValueLayout->height;
    buildRect(left, top, width, height, glm::vec3(0.0f), 0.6f);
    char scoreStr[32];
    itoa(GameLogic::curScore, scoreStr, 10);
    buildTextMesh(left, top, width, height, scoreStr, height, 
                  Palette::scoreBarText, 1.0f, 0.0f, haCenter, vaCenter);
  }

  // 'menu' button
  if (LayoutObject * scoreBarMenuButtonLayout = Layout::screen.getChildRecursive(loScoreBarMenuButton))
  {
    const float left = scoreBarMenuButtonLayout->getGlobalLeft();
    const float top = scoreBarMenuButtonLayout->getGlobalTop();
    const float width = scoreBarMenuButtonLayout->width;
    const float height = scoreBarMenuButtonLayout->height;
    const float border = 0.125f * height;
    const float textHeight = 0.75f * height;
    const glm::vec3 bkColor = GameLogic::menuButtonHighlighted ? 
                              Palette::scoreBarMenuButtonHighlightedBackground : 
                              Palette::scoreBarMenuButtonBackground;
    const glm::vec3 fgColor = GameLogic::menuButtonHighlighted ? 
                              Palette::scoreBarMenuButtonHighlightedText : 
                              Palette::scoreBarMenuButtonText;
    buildRect(left + 0.5f * border, top + 0.5f * border, width - border, height - border, bkColor, 1.0f);
    buildFrameRect(left, top, width, height, border, fgColor, 1.0f);
    buildTextMesh(left, top, width, height, "MENU", textHeight, fgColor, 1.0f, 0.0f, haCenter, vaCenter);
  }

  // hold figure panel
  if (LayoutObject * holdPanelCaptionLayout = Layout::screen.getChildRecursive(loHoldPanelCaption))
  {
    const float left = holdPanelCaptionLayout->getGlobalLeft();
    const float top = holdPanelCaptionLayout->getGlobalTop();
    const float width = holdPanelCaptionLayout->width;
    const float height = holdPanelCaptionLayout->height;
    buildTextMesh(left, top, width, height, "HOLD", height, 
                  Palette::holdCaptionText, 1.0f, 0.0f, haCenter, vaCenter);
  }

  if (LayoutObject * holdPanelLayout = Layout::screen.getChildRecursive(loHoldPanel))
  {
    const float left = holdPanelLayout->getGlobalLeft();
    const float top = holdPanelLayout->getGlobalTop();
    const float width = holdPanelLayout->width;
    const float height = holdPanelLayout->height;
    const glm::vec3 & color = (GameLogic::state != GameLogic::stStopped && 
                               GameLogic::holdFigure.color != Cell::clNone) ?
                              Palette::cellColorArray[GameLogic::holdFigure.color] : 
                              Palette::holdEmptyPanel;
    buildTexturedRect(left, top, width, height, tiHoldBackground, color, 1.0f);
  }

  // next figure
  if (LayoutObject * nextPanelCaptionLayout = Layout::screen.getChildRecursive(loNextPanelCaption))
  {
    const float left = nextPanelCaptionLayout->getGlobalLeft();
    const float top = nextPanelCaptionLayout->getGlobalTop();
    const float width = nextPanelCaptionLayout->width;
    const float height = nextPanelCaptionLayout->height;
    buildTextMesh(left, top, width, height, "NEXT   ", height, 
                  Palette::nextCaptionText, 1.0f, 0.0f, haCenter, vaCenter);
  }

  if (LayoutObject * nextPanelLayout = Layout::screen.getChildRecursive(loNextPanel))
  {
    const float left = nextPanelLayout->getGlobalLeft();
    const float top = nextPanelLayout->getGlobalTop();
    const float width = nextPanelLayout->width;
    const float height = nextPanelLayout->height;
    const glm::vec3 & color = (GameLogic::state != GameLogic::stStopped && 
                               GameLogic::nextFigures[0].color != Cell::clNone) ?
                              Palette::cellColorArray[GameLogic::nextFigures[0].color] : 
                              Palette::nextEmptyPanel;
    buildTexturedRect(left, top, width, height, tiNextBackground, color, 1.0f);
  }

  // level panel
  if (LayoutObject * levelPanelCaptionLayout = Layout::screen.getChildRecursive(loLevelPanelCaption))
  {
    const float left = levelPanelCaptionLayout->getGlobalLeft();
    const float top = levelPanelCaptionLayout->getGlobalTop();
    const float width = levelPanelCaptionLayout->width;
    const float height = levelPanelCaptionLayout->height;
    buildTextMesh(left, top, width, height, "LEVEL", height, 
                  Palette::levelCaptionText, 1.0f, 0.0f, haCenter, vaCenter);
  }

  if (LayoutObject * levelPanelLayout = Layout::screen.getChildRecursive(loLevelPanel))
  {
    const float left = levelPanelLayout->getGlobalLeft();
    const float top = levelPanelLayout->getGlobalTop();
    const float width = levelPanelLayout->width;
    const float height = levelPanelLayout->height;
    buildTexturedRect(left, top, width, height, tiLevelGoalBackground, 
                      Palette::levelPanelBackground, 1.0f);
    char levelStr[32];
    itoa(GameLogic::curLevel, levelStr, 10);
    buildTextMesh(left, top, width, height, levelStr, Layout::levelGoalTextHeight,
                  Palette::levelPanelText, 1.0f, 0.0f, haCenter, vaCenter);
  }

  // goal panel
  if (LayoutObject * goalPanelCaptionLayout = Layout::screen.getChildRecursive(loGoalPanelCaption))
  {
    const float left = goalPanelCaptionLayout->getGlobalLeft();
    const float top = goalPanelCaptionLayout->getGlobalTop();
    const float width = goalPanelCaptionLayout->width;
    const float height = goalPanelCaptionLayout->height;
    buildTextMesh(left, top, width, height, "GOAL", height, 
                  Palette::goalCaptionText, 1.0f, 0.0f, haCenter, vaCenter);
  }

  if (LayoutObject * goalPanelLayout = Layout::screen.getChildRecursive(loGoalPanel))
  {
    const float left = goalPanelLayout->getGlobalLeft();
    const float top = goalPanelLayout->getGlobalTop();
    const float width = goalPanelLayout->width;
    const float height = goalPanelLayout->height;
    buildTexturedRect(left, top, width, height, tiLevelGoalBackground, 
                      Palette::goalPanelBackground, 1.0f);
    char goalStr[32];
    itoa(GameLogic::curGoal, goalStr, 10);
    buildTextMesh(left, top, width, height, goalStr, Layout::levelGoalTextHeight, 
                  Palette::goalPanelText, 1.0f, 0.0f, haCenter, vaCenter);
  }
}


void OpenGLRender::buidField()
{
  if (LayoutObject * fieldLayout = Layout::screen.getChildRecursive(loField))
  {
    const float scale = fieldLayout->width / Field::width;
    const glm::vec2 fieldPos(fieldLayout->getGlobalLeft(), fieldLayout->getGlobalTop());
    glm::vec2 origin = fieldPos;

    if(!GameLogic::haveFallingRows)
      GameLogic::storeCurFigureIntoField();
      
    for (int y = 0; y < Field::height; y++)
    {
      if (GameLogic::getRowElevation(y))
        origin.y = fieldPos.y - scale * GameLogic::getRowCurrentElevation(y);
      else
        origin.y = fieldPos.y;

      for (int x = 0; x < Field::width; x++)
        buildCellShadow(origin, scale, GameLogic::field, x, y, true);
    }

    for (int y = 0; y < Field::height; y++)
    {
      if (GameLogic::getRowElevation(y))
        origin.y = fieldPos.y - scale * GameLogic::getRowCurrentElevation(y);
      else
        origin.y = fieldPos.y;

      for (int x = 0; x < Field::width; x++)
        buildCell(origin, scale, GameLogic::field, x, y, false);
    }

    for (int y = 0; y < Field::height; y++)
    {
      if (GameLogic::getRowElevation(y))
        origin.y = fieldPos.y - scale * GameLogic::getRowCurrentElevation(y);
      else
        origin.y = fieldPos.y;

      for (int x = 0; x < Field::width; x++)
        buildCellGlow(origin, scale, GameLogic::field, x, y, true);
    }

    if (!GameLogic::haveFallingRows)
      GameLogic::removeCurFigureFromField();
  }
}


void OpenGLRender::buildHoldFigure()
{
  if (GameLogic::haveHold)
  {
    if (LayoutObject * holdPanelLayout = Layout::screen.getChildRecursive(loHoldPanel))
    {
      const float scale = Layout::holdNextFigureScale;
      const float holdPanelLeft = holdPanelLayout->getGlobalLeft();
      const float holdPanelTop = holdPanelLayout->getGlobalTop();
      const float holdPanelWidth = holdPanelLayout->width;
      const float holdPanelHeight = holdPanelLayout->height;
      const Figure & figure = GameLogic::holdFigure;

      glm::vec2 origin(holdPanelLeft + 0.5f * holdPanelWidth, holdPanelTop + 0.5f * holdPanelHeight);
      origin -= scale * figure.getCenterPos();

      for (int y = 0; y < figure.dim; y++)
        for (int x = 0; x < figure.dim; x++)
          buildCell(origin, scale, figure, x, y, true);

      for (int y = 0; y < figure.dim; y++)
        for (int x = 0; x < figure.dim; x++)
          buildCellGlow(origin, scale, figure, x, y, false);
    }
  }
}


void OpenGLRender::buildNextFigures()
{
  if (LayoutObject * nextPanelLayout = Layout::screen.getChildRecursive(loNextPanel))
  {
    const float scale = Layout::holdNextFigureScale;
    const float nextPanelLeft = nextPanelLayout->getGlobalLeft();
    const float nextPanelTop = nextPanelLayout->getGlobalTop();
    const float nextPanelWidth = nextPanelLayout->width;
    const float nextPanelHeight = nextPanelLayout->height;

    for (int i = 0; i < GameLogic::nextFiguresCount; ++i)
    {
      const Figure & figure = GameLogic::nextFigures[i];

      glm::vec2 origin(nextPanelLeft + 0.5f * nextPanelWidth, nextPanelTop + (0.5f + i) * nextPanelHeight);
      origin -= scale * figure.getCenterPos();

      for (int y = 0; y < figure.dim; y++)
        for (int x = 0; x < figure.dim; x++)
          buildCell(origin, scale, figure, x, y, true);

      for (int y = 0; y < figure.dim; y++)
        for (int x = 0; x < figure.dim; x++)
          buildCellGlow(origin, scale, figure, x, y, false);
    }
  }
}


void OpenGLRender::buildCell(const glm::vec2 & origin, float scale, const CellArray & cells, int x, int y, bool bold)
{
  const Cell * cell = cells.getCell(x, y);

  if (cell && cell->figureId)
  {
    const int cellId = cell->figureId;

    for (int i = 0; i < 4; i++)
    {
      int cornerDX = (i & 1) * 2 - 1;
      int cornerDY = (i & 2) - 1;

      const Cell * horzAdjCell = cells.getCell(x + cornerDX, y);
      const Cell * vertAdjCell = cells.getCell(x, y + cornerDY);
      const Cell * cornerAdjCell = cells.getCell(x + cornerDX, y + cornerDY);

      bool haveHorzAdjCell = (horzAdjCell && horzAdjCell->figureId == cellId);
      bool haveVertAdjCell = (vertAdjCell && vertAdjCell->figureId == cellId);
      bool haveCornerAdjCell = (cornerAdjCell && cornerAdjCell->figureId == cellId);

      static const glm::vec2 openSegmentUV[3] =
      {
        { 0.5f, 0.5f },
        { 0.5f, 1.0f },
        { 0.0f, 1.0f },
      };

      static const glm::vec2 partialSegmentUV[3] =
      {
        { 0.5f, 0.5f },
        { 0.0f, 0.5f },
        { 0.0f, 0.0f },
      };

      static const glm::vec2 closedSegmentUV[3] =
      {
        { 0.5f, 0.5f },
        { 0.5f, 0.0f },
        { 0.0f, 0.0f },
      };

      const glm::vec2 * horzSegmentUV = NULL;
      const glm::vec2 * vertSegmentUV = NULL;

      if (haveHorzAdjCell && haveVertAdjCell && haveCornerAdjCell)
      {
        horzSegmentUV = openSegmentUV;
        vertSegmentUV = openSegmentUV;
      }
      else
      {
        horzSegmentUV = haveHorzAdjCell ? partialSegmentUV : closedSegmentUV;
        vertSegmentUV = haveVertAdjCell ? partialSegmentUV : closedSegmentUV;
      }

      const float dx = float(i & 1);
      const float dy = float((i & 2) >> 1);

      glm::vec2 verts[4] =
      {
        { x + 0.5f, y + 0.5f },
        { x + 0.5f, y + dy },
        { x + dx,   y + dy },
        { x + dx,   y + 0.5f },
      };

      const glm::vec3 & color = Palette::cellColorArray[cell->color];
      int texIndex = bold ? tiFigureCellBold : tiFigureCellNormal;

      addAtlasVertex(origin + scale * verts[0], vertSegmentUV[0], texIndex, color, 1.0f);
      addAtlasVertex(origin + scale * verts[1], vertSegmentUV[1], texIndex, color, 1.0f);
      addAtlasVertex(origin + scale * verts[2], vertSegmentUV[2], texIndex, color, 1.0f);
      addAtlasVertex(origin + scale * verts[0], horzSegmentUV[0], texIndex, color, 1.0f);
      addAtlasVertex(origin + scale * verts[3], horzSegmentUV[1], texIndex, color, 1.0f);
      addAtlasVertex(origin + scale * verts[2], horzSegmentUV[2], texIndex, color, 1.0f);
    }
  }
}


void OpenGLRender::buildCellShadow(const glm::vec2 & origin, float scale, const CellArray & cells, int x, int y, bool crop)
{
  const float shadowWidth = 0.15f;
  const float innerOffset = 2.0f / atlasSpriteSize;
  const Cell * cell = cells.getCell(x, y);

  if (cell->figureId)
  {
    const Cell * rightCell = cells.getCell(x + 1, y);
    const Cell * rightBottomCell = cells.getCell(x + 1, y + 1);
    const Cell * bottomCell = cells.getCell(x, y + 1);

    if (bottomCell && bottomCell->figureId != cell->figureId)
    {
      const Cell * leftCell = cells.getCell(x - 1, y);
      const Cell * bottomLeftCell = cells.getCell(x - 1, y + 1);
      bool softLeft = !leftCell || leftCell->figureId != cell->figureId;

      glm::vec2 verts[4] =
      {
        { x,        y + 1.0f - innerOffset },
        { x,        y + 1.0f + shadowWidth },
        { x + 1.0f, y + 1.0f - innerOffset },
        { x + 1.0f, y + 1.0f + shadowWidth }
      };

      glm::vec2 uv[4] =
      {
        { softLeft ? 1.0f : 0.5f, 0.5f },
        { 1.0f,                   1.0f },
        { 0.5f,                   0.5f },
        { 1.0f,                   1.0f }
      };

      if (softLeft)
        verts[1].x += shadowWidth;

      if (bottomLeftCell && bottomLeftCell->figureId == cell->figureId)
      {
        verts[0].x -= innerOffset;
        verts[1].x += shadowWidth;
      }

      if (rightBottomCell && rightBottomCell->figureId != cell->figureId &&
          rightCell && rightCell->figureId != cell->figureId)
      {
        verts[2].x -= innerOffset;
        verts[3].x += shadowWidth;
      }

      addAtlasVertex(origin + scale * verts[0], uv[0], tiFigureShadow, Palette::figureShadow, 1.0f);
      addAtlasVertex(origin + scale * verts[1], uv[1], tiFigureShadow, Palette::figureShadow, 1.0f);
      addAtlasVertex(origin + scale * verts[2], uv[2], tiFigureShadow, Palette::figureShadow, 1.0f);
      addAtlasVertex(origin + scale * verts[1], uv[1], tiFigureShadow, Palette::figureShadow, 1.0f);
      addAtlasVertex(origin + scale * verts[2], uv[2], tiFigureShadow, Palette::figureShadow, 1.0f);
      addAtlasVertex(origin + scale * verts[3], uv[3], tiFigureShadow, Palette::figureShadow, 1.0f);
    }

    if (rightCell && rightCell->figureId != cell->figureId)
    {
      const Cell * topCell = cells.getCell(x, y - 1);
      const Cell * topRightCell = cells.getCell(x + 1, y - 1);
      bool softTop = !topCell || topCell->figureId != cell->figureId;

      glm::vec2 verts[4] =
      {
        { x + 1.0f - innerOffset, y },
        { x + 1.0f + shadowWidth, y },
        { x + 1.0f - innerOffset, y + 1.0f },
        { x + 1.0f + shadowWidth, y + 1.0f }
      };

      glm::vec2 uv[4] =
      {
        { softTop ? 1.0f : 0.5f, 0.5f },
        { 1.0f,                  1.0f },
        { 0.5f,                  0.5f },
        { 1.0f,                  1.0f }
      };

      if (softTop)
        verts[1].y += shadowWidth;

      if (topRightCell && topRightCell->figureId == cell->figureId)
      {
        verts[0].y -= innerOffset;
        verts[1].y += shadowWidth;
      }

      if (rightBottomCell && rightBottomCell->figureId != cell->figureId &&
          bottomCell && bottomCell->figureId != cell->figureId)
      {
        verts[2].y -= innerOffset;
        verts[3].y += shadowWidth;
      }

      addAtlasVertex(origin + scale * verts[0], uv[0], tiFigureShadow, Palette::figureShadow, 1.0f);
      addAtlasVertex(origin + scale * verts[1], uv[1], tiFigureShadow, Palette::figureShadow, 1.0f);
      addAtlasVertex(origin + scale * verts[2], uv[2], tiFigureShadow, Palette::figureShadow, 1.0f);
      addAtlasVertex(origin + scale * verts[1], uv[1], tiFigureShadow, Palette::figureShadow, 1.0f);
      addAtlasVertex(origin + scale * verts[2], uv[2], tiFigureShadow, Palette::figureShadow, 1.0f);
      addAtlasVertex(origin + scale * verts[3], uv[3], tiFigureShadow, Palette::figureShadow, 1.0f);
    }
  }
}


void OpenGLRender::buildCellGlow(const glm::vec2 & origin, float scale, const CellArray & cells, int x, int y, bool crop)
{
  const float innerOffset = 2.0f / atlasSpriteSize;
  const float glowWidth = 0.5f;
  const glm::vec2 centerUV(0.5f);
  const Cell * cell = cells.getCell(x, y);

  if (cell->figureId)
  {
    const Cell * leftCell = cells.getCell(x - 1, y);
    const Cell * leftTopCell = cells.getCell(x - 1, y - 1);
    const Cell * topCell = cells.getCell(x, y - 1);
    const Cell * topRightCell = cells.getCell(x + 1, y - 1);
    const Cell * rightCell = cells.getCell(x + 1, y);
    const Cell * rightBottomCell = cells.getCell(x + 1, y + 1);
    const Cell * bottomCell = cells.getCell(x, y + 1);
    const Cell * bottomLeftCell = cells.getCell(x - 1, y + 1);

    bool haveLeftCell = leftCell && leftCell->figureId == cell->figureId;
    bool haveLeftTopCell = leftTopCell && leftTopCell->figureId == cell->figureId;
    bool haveTopCell = topCell && topCell->figureId == cell->figureId;
    bool haveTopRightCell = topRightCell && topRightCell->figureId == cell->figureId;
    bool haveRightCell = rightCell && rightCell->figureId == cell->figureId;
    bool haveRightBottomCell = rightBottomCell && rightBottomCell->figureId == cell->figureId;
    bool haveBottomCell = bottomCell && bottomCell->figureId == cell->figureId;
    bool haveBottomLeftCell = bottomLeftCell && bottomLeftCell->figureId == cell->figureId;

    const glm::vec3 & glowColor = Palette::cellColorArray[cell->color];
    const glm::vec3 glowInnerColor = glowColor * Palette::figureGlowInnerBright;
    const glm::vec3 glowOuterColor = glowColor * Palette::figureGlowOuterBright;

    if ((leftCell && leftCell->figureId != cell->figureId) ||
        (!leftCell && !crop))
    {
      glm::vec2 verts[4] =
      {
        { x + innerOffset, y },
        { x - glowWidth,   y },
        { x + innerOffset, y + 1.0f },
        { x - glowWidth,   y + 1.0f }
      };

      if (haveLeftTopCell)
      {
        verts[0].y -= innerOffset;
        verts[1].y += glowWidth;
      }
      else if ((topCell || !crop) && !haveTopCell)
      {
        verts[0].y += innerOffset;
        verts[1].y -= glowWidth;
      }

      if (haveBottomLeftCell)
      {
        verts[2].y += innerOffset;
        verts[3].y -= glowWidth;
      }
      else if ((bottomCell || !crop) && !haveBottomCell)
      {
        verts[2].y -= innerOffset;
        verts[3].y += glowWidth;
      }

      addAtlasVertex(origin + scale * verts[0], centerUV, tiEmpty, glowInnerColor, 0.0f);
      addAtlasVertex(origin + scale * verts[1], centerUV, tiEmpty, glowOuterColor, 0.0f);
      addAtlasVertex(origin + scale * verts[2], centerUV, tiEmpty, glowInnerColor, 0.0f);
      addAtlasVertex(origin + scale * verts[1], centerUV, tiEmpty, glowOuterColor, 0.0f);
      addAtlasVertex(origin + scale * verts[2], centerUV, tiEmpty, glowInnerColor, 0.0f);
      addAtlasVertex(origin + scale * verts[3], centerUV, tiEmpty, glowOuterColor, 0.0f);
    }

    if ((rightCell && rightCell->figureId != cell->figureId) ||
       (!rightCell && !crop))
    {
      glm::vec2 verts[4] =
      {
        { x + 1.0f - innerOffset, y },
        { x + 1.0f + glowWidth,   y },
        { x + 1.0f - innerOffset, y + 1.0f },
        { x + 1.0f + glowWidth,   y + 1.0f }
      };

      if (haveTopRightCell)
      {
        verts[0].y -= innerOffset;
        verts[1].y += glowWidth;
      }
      else if ((topCell || !crop) && !haveTopCell)
      {
        verts[0].y += innerOffset;
        verts[1].y -= glowWidth;
      }

      if (haveRightBottomCell)
      {
        verts[2].y += innerOffset;
        verts[3].y -= glowWidth;
      }
      else if ((bottomCell || !crop) && !haveBottomCell)
      {
        verts[2].y -= innerOffset;
        verts[3].y += glowWidth;
      }

      addAtlasVertex(origin + scale * verts[0], centerUV, tiEmpty, glowInnerColor, 0.0f);
      addAtlasVertex(origin + scale * verts[1], centerUV, tiEmpty, glowOuterColor, 0.0f);
      addAtlasVertex(origin + scale * verts[2], centerUV, tiEmpty, glowInnerColor, 0.0f);
      addAtlasVertex(origin + scale * verts[1], centerUV, tiEmpty, glowOuterColor, 0.0f);
      addAtlasVertex(origin + scale * verts[2], centerUV, tiEmpty, glowInnerColor, 0.0f);
      addAtlasVertex(origin + scale * verts[3], centerUV, tiEmpty, glowOuterColor, 0.0f);
    }

    if ((topCell && topCell->figureId != cell->figureId) ||
       (!topCell && !crop))
    {
      glm::vec2 verts[4] =
      {
        { x,        y + innerOffset },
        { x,        y - glowWidth },
        { x + 1.0f, y + innerOffset },
        { x + 1.0f, y - glowWidth }
      };

      if (haveLeftTopCell)
      {
        verts[0].x -= innerOffset;
        verts[1].x += glowWidth;
      }
      else if ((leftCell || !crop) && !haveLeftCell)
      {
        verts[0].x += innerOffset;
        verts[1].x -= glowWidth;
      }

      if (haveTopRightCell)
      {
        verts[2].x += innerOffset;
        verts[3].x -= glowWidth;
      }
      else if ((rightCell || !crop) && !haveRightCell)
      {
        verts[2].x -= innerOffset;
        verts[3].x += glowWidth;
      }

      addAtlasVertex(origin + scale * verts[0], centerUV, tiEmpty, glowInnerColor, 0.0f);
      addAtlasVertex(origin + scale * verts[1], centerUV, tiEmpty, glowOuterColor, 0.0f);
      addAtlasVertex(origin + scale * verts[2], centerUV, tiEmpty, glowInnerColor, 0.0f);
      addAtlasVertex(origin + scale * verts[1], centerUV, tiEmpty, glowOuterColor, 0.0f);
      addAtlasVertex(origin + scale * verts[2], centerUV, tiEmpty, glowInnerColor, 0.0f);
      addAtlasVertex(origin + scale * verts[3], centerUV, tiEmpty, glowOuterColor, 0.0f);
    }

    if ((bottomCell && bottomCell->figureId != cell->figureId) ||
       (!bottomCell && !crop))
    {
      glm::vec2 verts[4] =
      {
        { x,        y + 1.0f - innerOffset },
        { x,        y + 1.0f + glowWidth },
        { x + 1.0f, y + 1.0f - innerOffset },
        { x + 1.0f, y + 1.0f + glowWidth }
      };

      if (haveBottomLeftCell)
      {
        verts[0].x -= innerOffset;
        verts[1].x += glowWidth;
      }
      else if ((leftCell || !crop) && !haveLeftCell)
      {
        verts[0].x += innerOffset;
        verts[1].x -= glowWidth;
      }

      if (haveRightBottomCell)
      {
        verts[2].x += innerOffset;
        verts[3].x -= glowWidth;
      }
      else if ((rightCell || !crop) && !haveRightCell)
      {
        verts[2].x -= innerOffset;
        verts[3].x += glowWidth;
      }

      addAtlasVertex(origin + scale * verts[0], centerUV, tiEmpty, glowInnerColor, 0.0f);
      addAtlasVertex(origin + scale * verts[1], centerUV, tiEmpty, glowOuterColor, 0.0f);
      addAtlasVertex(origin + scale * verts[2], centerUV, tiEmpty, glowInnerColor, 0.0f);
      addAtlasVertex(origin + scale * verts[1], centerUV, tiEmpty, glowOuterColor, 0.0f);
      addAtlasVertex(origin + scale * verts[2], centerUV, tiEmpty, glowInnerColor, 0.0f);
      addAtlasVertex(origin + scale * verts[3], centerUV, tiEmpty, glowOuterColor, 0.0f);
    }
  }
}


void OpenGLRender::buildDropTrails()
{
  if (LayoutObject * fieldLayout = Layout::screen.getChildRecursive(loField))
  {
    const float left = fieldLayout->getGlobalLeft();
    const float top = fieldLayout->getGlobalTop();
    const float scale = fieldLayout->width / Field::width;

    for (int trailInd = GameLogic::dropTrailsTail; 
         trailInd != GameLogic::dropTrailsHead; 
         trailInd = (trailInd + 1) % GameLogic::dropTrailsSize)
    {
      DropTrail & dropTrail = GameLogic::dropTrails[trailInd];

      float trailProgress = dropTrail.getTrailProgress();
      float trailOpSqProgress = 1.0f - trailProgress * trailProgress;
      float trailLeft = left + scale * dropTrail.x - scale * 0.25f;
      float trailTop = top + scale * (dropTrail.y - dropTrail.height * trailOpSqProgress);
      float trailWidth = scale * 1.5f;
      float trailHeight = scale * (0.1f + 1.1f * dropTrail.height * trailOpSqProgress);
      glm::vec3 trailColor = Palette::cellColorArray[dropTrail.color] * trailOpSqProgress;

      buildTexturedRect(trailLeft, trailTop, trailWidth, trailHeight, tiDropTrail, trailColor, 0.0f);

      float sparklesProgress = dropTrail.getSparklesProgress();
      float sparklesOpSqProgress = 1.0f - sparklesProgress * sparklesProgress;

      for (int sparkleInd = 0; sparkleInd < DropTrail::sparkleQty; sparkleInd++)
      {
        const DropSparkle & sparkle = dropTrail.sparkles[sparkleInd];
        float sparkleX = scale * (dropTrail.x + sparkle.relX);
        float sparkleY = scale * (dropTrail.y - sparkle.relY * dropTrail.height - 
                                  sparkle.speed * sparklesProgress);
        const float sparkleSize = scale * 0.07f;
        glm::vec3 sparkleColor = 
          sparkle.alpha * (0.5f + Palette::cellColorArray[dropTrail.color]) * sparklesOpSqProgress;

        if (sparkleX < Field::width - sparkleSize && sparkleY > 0.0f)
          buildTexturedRect(left + sparkleX, top + sparkleY, sparkleSize, sparkleSize, 
                            tiDropSparkle, sparkleColor, 0.0f);
      }
    }
  }
}


void OpenGLRender::buildRowFlashes()
{
  if (LayoutObject * fieldLayout = Layout::screen.getChildRecursive(loField))
  {
    const float fieldLeft = fieldLayout->getGlobalLeft();
    const float fieldTop = fieldLayout->getGlobalTop();
    const float scale = fieldLayout->width / Field::width;
    float overallProgress = glm::clamp(float(Time::timer - GameLogic::rowsDeleteTimer) /
                                       GameLogic::rowsDeletionEffectTime, 0.0f, 1.0f);
    float mul = 1.0f - cos((overallProgress - 0.5f) * 
                           (overallProgress < 0.5f ? 0.5f : 2.0f) * 
                           (float)M_PI_2);
    float dx = 1.0f - mul * 3.0f;
    float dy = 0.25f - 0.75f * mul;
    glm::vec3 flashColor(Palette::deletedRowFlashBright * (1.0f - overallProgress * overallProgress));

    for (GameLogic::DeletedRowsIterator delRowIt = GameLogic::getDeletedRowsBegin(), 
         end = GameLogic::getDeletedRowsEnd(); delRowIt != end; ++delRowIt)
    {
      int row = *delRowIt;
      float flashLeft = fieldLeft - scale * dx;
      float flashTop = fieldTop + scale * (row - dy);
      float flashWidth = scale * (Field::width + 2.0f * dx);
      float flashHeight = scale * (1.0f + 2.0f * dy);
      buildTexturedRect(flashLeft, flashTop, flashWidth, flashHeight, tiRowFlash, flashColor, 0.0f);
    }

    if (GameLogic::getDeletedRowsBegin() != GameLogic::getDeletedRowsEnd())
    {
      float shineProgress = glm::clamp(overallProgress * 1.2f, 0.0f, 1.0f);
      int firstRow = *GameLogic::getDeletedRowsBegin();
      int lastRow = *(GameLogic::getDeletedRowsEnd() - 1);

      float lightSourceX = (shineProgress - 0.5f) * 3.0f * Field::width;
      float lightSourceY = 0.5f * (firstRow + lastRow + 1);

      for (GameLogic::DeletedRowGapsIterator rowGapsIt = GameLogic::getDeletedRowGapsBegin(); 
           rowGapsIt != GameLogic::getDeletedRowGapsEnd(); 
           ++rowGapsIt)
      {
        float gapX = float(rowGapsIt->x);
        float gapY1 = float(rowGapsIt->y);
        float gapY2 = float(rowGapsIt->y + 1);
        float ltDX = gapX - lightSourceX;
        float ltDY1 = gapY1 - lightSourceY;
        float ltDY2 = gapY2 - lightSourceY;
        const float leverRatio = 8.0f;
        float rayEndDX = ltDX * leverRatio;
        float rayEndDY1 = ltDY1 * leverRatio;
        float rayEndDY2 = ltDY2 * leverRatio;

        glm::vec2 uv[4] =
        {
          { 0.5f, 0.0f },
          { 0.0f, 1.0f },
          { 1.0f, 1.0f },
        };

        float alphaMul = 
          1.0f - 2.0f * fabs(0.5f - glm::clamp(shineProgress * shineProgress, 0.1f, 1.0f));
        glm::vec3 rayBeginColor(0.2f * alphaMul * Palette::deletedRowRaysBright);
        glm::vec3 rayEndColor(0.0f);

        glm::vec2 verts[3] =
        {
          { fieldLeft + scale * lightSourceX, fieldTop + scale * lightSourceY },
          { fieldLeft + scale * (gapX + rayEndDX), fieldTop + scale * (gapY1 + rayEndDY1) },
          { fieldLeft + scale * (gapX + rayEndDX), fieldTop + scale * (gapY2 + rayEndDY2) },
        };

        addAtlasVertex(verts[0], uv[0], tiRowShineRay, rayBeginColor, 0.0f);
        addAtlasVertex(verts[1], uv[1], tiRowShineRay, rayEndColor, 0.0f);
        addAtlasVertex(verts[2], uv[2], tiRowShineRay, rayEndColor, 0.0f);
      }

      const float shineSize = 3.5f;
      float shineLeft = fieldLeft + scale * lightSourceX - 0.5f * shineSize;
      float shineTop = fieldTop + scale * lightSourceY - 0.5f * shineSize;
      const glm::vec3 shineColor(0.1f * sin(shineProgress * (float)M_PI) * Palette::deletedRowShineBright);

      buildTexturedRect(shineLeft, shineTop, shineSize, shineSize, tiRowShineLight, shineColor, 0.0f);
    }
  }
}


void OpenGLRender::buildSideBar(float left, float top, float width, float height, float cornerSize, 
                                float glowWidth, const glm::vec3 & topColor, 
                                const glm::vec3 & bottomColor, const glm::vec3 & glowColor)
{
  assert(width >= VERY_SMALL_NUMBER);
  assert(height >= VERY_SMALL_NUMBER);
  assert(cornerSize >= VERY_SMALL_NUMBER);

  if (width < VERY_SMALL_NUMBER || height < VERY_SMALL_NUMBER || cornerSize < VERY_SMALL_NUMBER)
    return;

  glm::vec2 origin = glm::vec2(left, top);
  const float tessSize = cornerSize;
  int xTess = int(width / tessSize);
  int yTess = int(height / tessSize);
  const float lastChunkWidth = width - xTess * tessSize;
  const float lastChunkHeight = height - yTess * tessSize;
  xTess++;
  yTess++;

  // 0:0 - the top right corner
  for (int y = 0; y < yTess; y++)
  {
    for (int x = 0; x < xTess; x++)
    {
      const float x0 = width - x * tessSize;
      const float y0 = y * tessSize;
      const float x1 = (x == xTess - 1) ?
                       width - x * tessSize - lastChunkWidth :
                       width - (x + 1) * tessSize;
      const float y1 = (y == yTess - 1) ?
                       y * tessSize + lastChunkHeight :
                       (y + 1) * tessSize;

      const glm::vec2 verts[4] =
      {
        { x0, y0 },
        { x1, y0 },
        { x0, y1 },
        { x1, y1 },
      };

      const float u0 = x0 / Layout::sidePanelBkTileWidth;
      const float v0 = y0 / Layout::sidePanelBkTileHeight;
      const float u1 = x1 / Layout::sidePanelBkTileWidth;
      const float v1 = y1 / Layout::sidePanelBkTileHeight;

      glm::vec2 uv[4] =
      {
        { u0, v0 },
        { u1, v0 },
        { u0, v1 },
        { u1, v1 },
      };

      float lt0 = 1.0f - glm::clamp(2.0f * y0 / height, 0.0f, 1.0f);
      float lt1 = 1.0f - glm::clamp(2.0f * y1 / height, 0.0f, 1.0f);
      glm::vec3 col0 = bottomColor + (topColor - bottomColor) * lt0;
      glm::vec3 col1 = bottomColor + (topColor - bottomColor) * lt1;

      if (x || y)
      {
        addBkVertex(origin + verts[0], uv[0], col0, 1.0f);
        addBkVertex(origin + verts[1], uv[1], col0, 1.0f);
        addBkVertex(origin + verts[2], uv[2], col1, 1.0f);
      }

      addBkVertex(origin + verts[1], uv[1], col0, 1.0f);
      addBkVertex(origin + verts[2], uv[2], col1, 1.0f);
      addBkVertex(origin + verts[3], uv[3], col1, 1.0f);
    }
  }

  const float sin_22_5 = 0.414213562373f;
  const float topInnerGlowWidth = glm::min(0.5f * height, 0.1f);
  const float bottomInnerGlowWidth = glm::min(0.1f * height, 0.01f);

  glm::vec2 borderVerts[6] = 
  {
    { 0.0f,                         0.0f },
    { 0.75f * (width - cornerSize), 0.0f },
    { width - tessSize,             0.0f },
    { width,                        tessSize },
    { width,                        height},
    { 0.0f,                         height},
  };

  glm::vec2 outerGlowVerts[6] = 
  {
    { 0.0f,                                      -glowWidth },
    { 0.75f * (width - cornerSize),              -glowWidth },
    { width - cornerSize + glowWidth * sin_22_5, -glowWidth },
    { width + glowWidth,                          cornerSize - glowWidth * sin_22_5 },
    { width + glowWidth,                          height + glowWidth },
    { 0.0f,                                       height + glowWidth },
  };

  glm::vec2 innerGlowVerts[6] =
  {
    { 0.0f,                                              topInnerGlowWidth },
    { 0.75f * (width - cornerSize),                      topInnerGlowWidth },
    { width - cornerSize - topInnerGlowWidth * sin_22_5, topInnerGlowWidth },
    { width,                                             cornerSize + topInnerGlowWidth },
    { width - 0.5f * bottomInnerGlowWidth,               height - 0.5f * bottomInnerGlowWidth },
    { 0.0f,                                              height - bottomInnerGlowWidth },
  };

  glm::vec2 borderUV[6] =
  {
    { 0.0f, 0.5f },
    { 0.5f, 0.5f },
    { 0.75f, 0.5f },
    { 0.75f, 0.5f },
    { 1.0f, 0.5f },
    { 1.0f, 0.5f },
  };

  glm::vec2 outerGlowUV[6] =
  {
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.75f, 0.0f },
    { 0.75f, 0.0f },
    { 1.0f, 0.0f },
    { 1.0f, 0.0f },
  };

  glm::vec2 innerGlowUV[6] =
  {
    { 0.0f, 1.0f },
    { 0.5f, 1.0f },
    { 0.75f, 1.0f },
    { 0.75f, 1.0f },
    { 1.0f, 1.0f },
    { 1.0f, 1.0f },
  };

  for (int i = 0; i < 5; i++)
  {
    addAtlasVertex(origin + borderVerts[i], borderUV[i], tiGuiPanelGlow, glowColor, 0.0f);
    addAtlasVertex(origin + outerGlowVerts[i], outerGlowUV[i], tiGuiPanelGlow, glowColor, 0.0f);
    addAtlasVertex(origin + borderVerts[i + 1], borderUV[i + 1], tiGuiPanelGlow, glowColor, 0.0f);

    addAtlasVertex(origin + outerGlowVerts[i], outerGlowUV[i], tiGuiPanelGlow, glowColor, 0.0f);
    addAtlasVertex(origin + borderVerts[i + 1], borderUV[i + 1], tiGuiPanelGlow, glowColor, 0.0f);
    addAtlasVertex(origin + outerGlowVerts[i + 1], outerGlowUV[i + 1], tiGuiPanelGlow, glowColor, 0.0f);

    addAtlasVertex(origin + borderVerts[i], borderUV[i], tiGuiPanelGlow, glowColor, 0.0f);
    addAtlasVertex(origin + innerGlowVerts[i], innerGlowUV[i], tiGuiPanelGlow, glowColor, 0.0f);
    addAtlasVertex(origin + borderVerts[i + 1], borderUV[i + 1], tiGuiPanelGlow, glowColor, 0.0f);

    addAtlasVertex(origin + innerGlowVerts[i], innerGlowUV[i], tiGuiPanelGlow, glowColor, 0.0f);
    addAtlasVertex(origin + borderVerts[i + 1], borderUV[i + 1], tiGuiPanelGlow, glowColor, 0.0f);
    addAtlasVertex(origin + innerGlowVerts[i + 1], innerGlowUV[i + 1], tiGuiPanelGlow, glowColor, 0.0f);
  }
}


void OpenGLRender::buildWindow(float left, float top, float width, float height, float cornerSize, 
                               float glowWidth, const glm::vec3 & topColor, 
                               const glm::vec3 & bottomColor, const glm::vec3 & glowColor)
{
  glm::vec2 origin = glm::vec2(left, top);
  const float tessSize = cornerSize;
  int xTess = int(width / tessSize);
  int yTess = int(height / tessSize);
  const float lastChunkWidth = width - xTess * tessSize;
  const float lastChunkHeight = height - yTess * tessSize;
  xTess++;
  yTess++;

  // 0:0 - the top right corner
  for (int y = 0; y < yTess; y++)
  {
    for (int x = 0; x < xTess; x++)
    {
      const float x0 = width - x * tessSize;
      const float y0 = y * tessSize;
      const float x1 = (x == xTess - 1) ?
                       width - x * tessSize - lastChunkWidth :
                       width - (x + 1) * tessSize;
      const float y1 = (y == yTess - 1) ?
                       y * tessSize + lastChunkHeight :
                       (y + 1) * tessSize;

      const glm::vec2 verts[4] =
      {
        { x0, y0 },
        { x1, y0 },
        { x0, y1 },
        { x1, y1 },
      };

      const float u0 = x0 / Layout::sidePanelBkTileWidth;
      const float v0 = y0 / Layout::sidePanelBkTileHeight;
      const float u1 = x1 / Layout::sidePanelBkTileWidth;
      const float v1 = y1 / Layout::sidePanelBkTileHeight;

      glm::vec2 uv[4] =
      {
        { u0, v0 },
        { u1, v0 },
        { u0, v1 },
        { u1, v1 },
      };

      float lt0 = 1.0f - glm::clamp(2.0f * y0 / height, 0.0f, 1.0f);
      float lt1 = 1.0f - glm::clamp(2.0f * y1 / height, 0.0f, 1.0f);

      glm::vec3 col0 = bottomColor + (topColor - bottomColor) * lt0;
      glm::vec3 col1 = bottomColor + (topColor - bottomColor) * lt1;

      if (x || y)
      {
        addBkVertex(origin + verts[0], uv[0], col0, 1.0f);
        addBkVertex(origin + verts[1], uv[1], col0, 1.0f);
        addBkVertex(origin + verts[2], uv[2], col1, 1.0f);
      }

      addBkVertex(origin + verts[1], uv[1], col0, 1.0f);
      addBkVertex(origin + verts[2], uv[2], col1, 1.0f);
      addBkVertex(origin + verts[3], uv[3], col1, 1.0f);
    }
  }

  const float sin_22_5 = 0.414213562373f;
  const float topInnerGlowWidth = glm::min(0.5f * height, 0.1f);
  const float restInnerGlowWidth = glm::min(0.1f * height, 0.01f);

  glm::vec2 borderVerts[7] =
  {
    { 0.0f,                         0.0f },
    { 0.75f * (width - cornerSize), 0.0f },
    { width - tessSize,             0.0f },
    { width,                        tessSize },
    { width,                        height },
    { 0.0f,                         height },
    { 0.0f,                         0.0f },
  };

  glm::vec2 outerGlowVerts[7] =
  {
    { -glowWidth,                                -glowWidth },
    { 0.75f * (width - cornerSize),              -glowWidth },
    { width - cornerSize + glowWidth * sin_22_5, -glowWidth },
    { width + glowWidth,                          cornerSize - glowWidth * sin_22_5 },
    { width + glowWidth,                          height + glowWidth },
    { -glowWidth,                                 height + glowWidth },
    { -glowWidth,                                -glowWidth },
  };

  glm::vec2 innerGlowVerts[7] =
  {
    { restInnerGlowWidth,                                 topInnerGlowWidth },
    { 0.75f * (width - cornerSize),                       topInnerGlowWidth },
    { width - cornerSize - topInnerGlowWidth * sin_22_5,  topInnerGlowWidth },
    { width,                                              cornerSize + topInnerGlowWidth },
    { width - 0.5f * restInnerGlowWidth,                  height - 0.5f * restInnerGlowWidth },
    { restInnerGlowWidth,                                 height - restInnerGlowWidth },
    { restInnerGlowWidth,                                 topInnerGlowWidth },
  };

  glm::vec2 borderUV[7] =
  {
    { 0.0f, 0.5f },
    { 0.5f, 0.5f },
    { 0.75f, 0.5f },
    { 0.75f, 0.5f },
    { 1.0f, 0.5f },
    { 1.0f, 0.5f },
    { 0.0f, 0.5f },
  };

  glm::vec2 outerGlowUV[7] =
  {
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.75f, 0.0f },
    { 0.75f, 0.0f },
    { 1.0f, 0.0f },
    { 1.0f, 0.0f },
    { 0.0f, 0.0f },
  };

  glm::vec2 innerGlowUV[7] =
  {
    { 0.0f, 1.0f },
    { 0.5f, 1.0f },
    { 0.75f, 1.0f },
    { 0.75f, 1.0f },
    { 1.0f, 1.0f },
    { 1.0f, 1.0f },
    { 0.0f, 1.0f },
  };

  for (int i = 0; i < 6; i++)
  {
    addAtlasVertex(origin + borderVerts[i], borderUV[i], tiGuiPanelGlow, glowColor, 0.0f);
    addAtlasVertex(origin + outerGlowVerts[i], outerGlowUV[i], tiGuiPanelGlow, glowColor, 0.0f);
    addAtlasVertex(origin + borderVerts[i + 1], borderUV[i + 1], tiGuiPanelGlow, glowColor, 0.0f);

    addAtlasVertex(origin + outerGlowVerts[i], outerGlowUV[i], tiGuiPanelGlow, glowColor, 0.0f);
    addAtlasVertex(origin + borderVerts[i + 1], borderUV[i + 1], tiGuiPanelGlow, glowColor, 0.0f);
    addAtlasVertex(origin + outerGlowVerts[i + 1], outerGlowUV[i + 1], tiGuiPanelGlow, glowColor, 0.0f);

    addAtlasVertex(origin + borderVerts[i], borderUV[i], tiGuiPanelGlow, glowColor, 0.0f);
    addAtlasVertex(origin + innerGlowVerts[i], innerGlowUV[i], tiGuiPanelGlow, glowColor, 0.0f);
    addAtlasVertex(origin + borderVerts[i + 1], borderUV[i + 1], tiGuiPanelGlow, glowColor, 0.0f);

    addAtlasVertex(origin + innerGlowVerts[i], innerGlowUV[i], tiGuiPanelGlow, glowColor, 0.0f);
    addAtlasVertex(origin + borderVerts[i + 1], borderUV[i + 1], tiGuiPanelGlow, glowColor, 0.0f);
    addAtlasVertex(origin + innerGlowVerts[i + 1], innerGlowUV[i + 1], tiGuiPanelGlow, glowColor, 0.0f);
  }
}


void OpenGLRender::buildMenu(MenuLogic * menuLogic, LayoutObject * menuLayout)
{
  assert(menuLogic);
  assert(menuLayout);

  if (!menuLogic || !menuLayout)
    return;

  for (int row = 0, cnt = menuLogic->rowCount; row < cnt; row++)
  {
    const float rowLag = 0.2f;
    float rowProgress;

    switch (menuLogic->state)
    {
      case MenuLogic::stShowing:
        rowProgress = 1.0f - glm::clamp(menuLogic->transitionProgress * (1.0f + rowLag * cnt) -
                                        rowLag * row, 0.0f, 1.0f);
        break;

      case MenuLogic::stHiding:
        rowProgress = 1.0f - glm::clamp(menuLogic->transitionProgress * (1.0f + rowLag * cnt) -
                                        rowLag * (cnt - row), 0.0f, 1.0f);
        break;

      default:
        rowProgress = 1.0f - menuLogic->transitionProgress;
        break;
    }

    LayoutObject::Rect menuRowRect = menuLayout->getCellGlobalRect(row, 0);
    menuRowRect.left -= (menuRowRect.width + Layout::menuRowGlowWidth) * rowProgress * rowProgress;
    const bool highlight = (row == menuLogic->selectedRow);
    const glm::vec3 & panelTopColor = highlight ? 
                                      Palette::menuSelectedRowBackgroundTop : 
                                      Palette::menuNormalRowBackgroundTop;
    const glm::vec3 & panelBottomColor = highlight ? 
                                         Palette::menuSelectedRowBackgroundBottom : 
                                         Palette::menuNormalRowBackgroundBottom;
    const glm::vec3 & panelGlowColor = highlight ? 
                                       Palette::menuSelectedRowGlow : 
                                       Palette::menuNormalRowGlow;
    buildSideBar(menuRowRect.left, menuRowRect.top, menuRowRect.width, menuRowRect.height, 
                 Layout::menuRowCornerSize, Layout::menuRowGlowWidth, 
                 panelTopColor, panelBottomColor, panelGlowColor);
    menuRowRect.left += Layout::menuRowTextOffset;
    const float textHeight = 0.08f;
    const glm::vec3 & textColor = highlight ? 
                                  Palette::menuSelectedRowText : 
                                  Palette::menuNormalRowText;
    const char * text = menuLogic->getText(row);
    buildTextMesh(menuRowRect.left, menuRowRect.top - 0.0025f, menuRowRect.width, menuRowRect.height, 
                  text, Layout::menuFontHeight, glm::vec3(0.0f), 0.5f, 1.0f, haLeft, vaCenter);
    buildTextMesh(menuRowRect.left, menuRowRect.top, menuRowRect.width, menuRowRect.height, 
                  text, Layout::menuFontHeight, textColor, 1.0f, 0.0f, haLeft, vaCenter);
  }
}


float OpenGLRender::buildTextMesh(float left, float top, float width, float height, const char * str, 
                                  float size, const glm::vec3 & color, float alpha, float blur, 
                                  HorzAllign horzAllign, VertAllign vertAllign)
{
  assert(font.falloff() >= VERY_SMALL_NUMBER);

  if (size < VERY_SMALL_NUMBER || font.falloff() < VERY_SMALL_NUMBER)
    return 0.0f;

  float penX = 0;
  char leftChar = 0;
  static std::vector<glm::vec2> verts(1024);
  static std::vector<glm::vec2> uv(1024);
  verts.clear();
  uv.clear();

  for (const char * pch = str; *pch; pch++)
  {
    if (const SDFF_Glyph * glyph = font.getGlyph(*pch))
    {
      float kern = leftChar ? font.getKerning(leftChar, *pch) : 0.0f;
      leftChar = *pch;
      float leftTopX = penX + size * (kern + glyph->bearingX - font.falloff());
      float leftTopY = -size * (glyph->bearingY + font.falloff());
      float rightBottomX = leftTopX + size * (glyph->width + 2.0f * font.falloff());
      float rightBottomY = leftTopY + size * (glyph->height + 2.0f * font.falloff());

      verts.push_back({ leftTopX,     leftTopY });
      verts.push_back({ leftTopX,     rightBottomY });
      verts.push_back({ rightBottomX, leftTopY });
      verts.push_back({ rightBottomX, rightBottomY });

      uv.push_back({ glyph->left,  glyph->top });
      uv.push_back({ glyph->left,  glyph->bottom });
      uv.push_back({ glyph->right, glyph->top });
      uv.push_back({ glyph->right, glyph->bottom });

      penX += size * glyph->advance;
    }
    else 
      assert(0);
  }

  float meshWidth = 0.0f;

  if (!verts.empty())
  {
    meshWidth = verts.back().x - verts.front().x;
    const float vOffs = 0.2f;
    glm::vec2 origin(left, top + size * font.maxBearingY());

    if (horzAllign == haRight)
      origin.x += width - meshWidth;
    else if (horzAllign == haCenter)
      origin.x += 0.5f * (width - meshWidth) - verts.front().x;

    const float vOffset = -size * 0.05f;

    if (vertAllign == vaBottom)
      origin.y += height - size * font.maxBearingY();
    else if (vertAllign == vaCenter)
      origin.y += 0.5f * (height - size * font.maxBearingY() + vOffset);

    const float fontBlur = 0.4f * pxSize / (size * font.falloff()) * (1.0f - blur) + blur * 0.5f;

    for (int i = 0, cnt = (int)verts.size(); i < cnt; i+=4)
    {
      addTextVertex(origin + verts[i + 0], uv[i + 0], fontBlur, color, alpha);
      addTextVertex(origin + verts[i + 1], uv[i + 1], fontBlur, color, alpha);
      addTextVertex(origin + verts[i + 2], uv[i + 2], fontBlur, color, alpha);
      addTextVertex(origin + verts[i + 1], uv[i + 1], fontBlur, color, alpha);
      addTextVertex(origin + verts[i + 2], uv[i + 2], fontBlur, color, alpha);
      addTextVertex(origin + verts[i + 3], uv[i + 3], fontBlur, color, alpha);
    }
  }

  return meshWidth;
}


void OpenGLRender::buildSettingsWindow()
{
  if (LayoutObject * settingsLayout = Layout::screen.getChild(loSettings))
  {
    if (LayoutObject * settingsWindowLayout = settingsLayout->getChild(loSettingsWindow))
    {
      const float opProgress = 1.0f - InterfaceLogic::settingsLogic.transitionProgress;
      const float sqOpProgress = opProgress * opProgress;
      const float fullSettingsHeight = settingsWindowLayout->height + 2.0f * Layout::settingsGlowWidth;
      const float shift = -sqOpProgress * (fullSettingsHeight + settingsWindowLayout->getGlobalTop());

      const float left = settingsWindowLayout->getGlobalLeft();
      const float top = settingsWindowLayout->getGlobalTop() + shift;
      const float width = settingsWindowLayout->width;
      const float height = settingsWindowLayout->height;
      buildWindow(left, top, width, height, Layout::settingsCornerSize, Layout::settingsGlowWidth, 
                  Palette::settingsBackgroundTop, Palette::settingsBackgroundBottom, Palette::settingsGlow);

      if (LayoutObject * settingTitleShadowLayout = settingsWindowLayout->getChild(loSettingsTitleShadow))
      {
        const float left = settingTitleShadowLayout->getGlobalLeft();
        const float top = settingTitleShadowLayout->getGlobalTop() + shift;
        const float width = settingTitleShadowLayout->width;
        const float height = settingTitleShadowLayout->height;
        buildTextMesh(left, top, width, height, "SETTINGS", Layout::settingsTitleHeight, glm::vec3(0.0f), 
                      Palette::settingsTitleShadowAlpha, Palette::settingsTitleShadowBlur, haLeft, vaTop);
      }

      if (LayoutObject * settingTitleLayout = settingsWindowLayout->getChild(loSettingsTitle))
      {
        const float left = settingTitleLayout->getGlobalLeft();
        const float top = settingTitleLayout->getGlobalTop() + shift;
        const float width = settingTitleLayout->width;
        const float height = settingTitleLayout->height;
        buildTextMesh(left, top, width, height, "SETTINGS", Layout::settingsTitleHeight, 
                      Palette::settingsTitleText, 1.0f, 0.0f, haLeft, vaTop);
      }

      if (LayoutObject * settingPanelLayout = settingsWindowLayout->getChild(loSettingsPanel))
      {
        const float left = settingPanelLayout->getGlobalLeft();
        const float top = settingPanelLayout->getGlobalTop() + shift;
        const float width = settingPanelLayout->width;
        const float height = settingPanelLayout->height;
        buildVertGradientRect(left, top, width, height, Palette::settingsPanelBackgroundTop, 1.0f, 
                              Palette::settingsPanelBackgroundBottom, 1.0f);
        buildFrameRect(left, top, width, height, Layout::settingsPanelBorderWidth, 
                       Palette::settingsPanelBorder, 1.0f);

        if (LayoutObject * volumeTitleLayout = settingPanelLayout->getChild(loVolumeTitle))
        {
          const float left = volumeTitleLayout->getGlobalLeft();
          const float top = volumeTitleLayout->getGlobalTop() + shift;
          const float width = volumeTitleLayout->width;
          const float height = volumeTitleLayout->height;
          buildTextMesh(left, top, width, height, "VOLUME", height, Palette::settingsPanelTitleText, 
                        1.0f, 0.0f, haLeft, vaCenter);
        }

        if (LayoutObject * soundVolumeRowLayout = settingPanelLayout->getChild(loSoundVolume))
        {
          const float left = soundVolumeRowLayout->getGlobalLeft();
          const float top = soundVolumeRowLayout->getGlobalTop() + shift;
          const float width = soundVolumeRowLayout->width;
          const float height = soundVolumeRowLayout->height;
          bool rowSelected = 
            (InterfaceLogic::settingsLogic.selectedControl == SettingsLogic::ctrlSoundVolume);
          bool rowHighlighted = 
            (InterfaceLogic::settingsLogic.highlightedControl == SettingsLogic::ctrlSoundVolume);
          const glm::vec3 & bkColor = rowSelected ? Palette::settingsActiveRowBackground :
                                      rowHighlighted ? Palette::settingsMouseoverRowBackground :
                                      Palette::settingsInactiveRowBackground;
          const glm::vec3 & textColor = rowSelected ? Palette::settingsActiveRowText :
                                        rowHighlighted ? Palette::settingsMouseoverRowText :
                                        Palette::settingsInactiveRowText;
          buildSmoothRect(left, top, width, height, edgeBlurWidth, bkColor, 1.0);
          buildTextMesh(left + Layout::settingsPanelRowCaptionIndent, top, width, height, "Sound", 
                        Layout::settingsPanelRowCaptionHeight, textColor, 1.0f, 0.0f, haLeft, vaCenter);

          if (LayoutObject * progressBarLayout = soundVolumeRowLayout->getChild(loSoundProgressBar))
          {
            const float left = progressBarLayout->getGlobalLeft();
            const float top = progressBarLayout->getGlobalTop() + shift;
            const float width = progressBarLayout->width;
            const float height = progressBarLayout->height;
            const glm::vec3 & bkColor = Palette::settingsProgressBarBackground;
            const glm::vec3 & barColor = Palette::settingsProgressBarForeground;
            const glm::vec3 & borderColor = rowSelected ? 
                                            Palette::settingsProgressBarBackground : 
                                            Palette::settingsProgressBarForeground;
            const float progress = InterfaceLogic::settingsLogic.getSoundVolume();
            buildProgressBar(left, top, width, height, bkColor, borderColor, barColor, 1.0f, progress);
          }
        }

        if (LayoutObject * musicVolumeRowLayout = settingPanelLayout->getChild(loMusicVolume))
        {
          const float left = musicVolumeRowLayout->getGlobalLeft();
          const float top = musicVolumeRowLayout->getGlobalTop() + shift;
          const float width = musicVolumeRowLayout->width;
          const float height = musicVolumeRowLayout->height;
          bool rowSelected = 
            (InterfaceLogic::settingsLogic.selectedControl == SettingsLogic::ctrlMusicVolume);
          bool rowHighlighted = 
            (InterfaceLogic::settingsLogic.highlightedControl == SettingsLogic::ctrlMusicVolume);
          const glm::vec3 & bkColor = rowSelected ? Palette::settingsActiveRowBackground : 
                                      rowHighlighted ? Palette::settingsMouseoverRowBackground : 
                                      Palette::settingsInactiveRowBackground;
          const glm::vec3 & textColor = rowSelected ? Palette::settingsActiveRowText :
                                        rowHighlighted ? Palette::settingsMouseoverRowText :
                                        Palette::settingsInactiveRowText;
          buildSmoothRect(left, top, width, height, edgeBlurWidth, bkColor, 1.0f);
          buildTextMesh(left + Layout::settingsPanelRowCaptionIndent, top, width, height, "Music", 
                        Layout::settingsPanelRowCaptionHeight, textColor, 1.0f, 0.0f, haLeft, vaCenter);

          if (LayoutObject * progressBarLayout = musicVolumeRowLayout->getChild(loMusicProgressBar))
          {
            const float left = progressBarLayout->getGlobalLeft();
            const float top = progressBarLayout->getGlobalTop() + shift;
            const float width = progressBarLayout->width;
            const float height = progressBarLayout->height;
            const glm::vec3 & bkColor = Palette::settingsProgressBarBackground;
            const glm::vec3 & barColor = Palette::settingsProgressBarForeground;
            const glm::vec3 & borderColor = rowSelected ?
                                            Palette::settingsProgressBarBackground :
                                            Palette::settingsProgressBarForeground;
            const float progress = InterfaceLogic::settingsLogic.getMusicVolume();
            buildProgressBar(left, top, width, height, bkColor, borderColor, barColor, 1.0f, progress);
          }
        }

        if (LayoutObject * keyBindingTitleLayout = settingPanelLayout->getChild(loKeyBindingTitle))
        {
          const float left = keyBindingTitleLayout->getGlobalLeft();
          const float top = keyBindingTitleLayout->getGlobalTop() + shift;
          const float width = keyBindingTitleLayout->width;
          const float height = keyBindingTitleLayout->height;
          buildTextMesh(left, top, width, height, "KEY BINDING", height, Palette::settingsPanelTitleText, 1.0f, 0.0f, haLeft, vaCenter);
        }

        if (LayoutObject * keyBindingGridLayout = settingPanelLayout->getChild(loKeyBindingGrid))
        {
          const int selectedRow =
            InterfaceLogic::settingsLogic.selectedControl == SettingsLogic::ctrlKeyBindTable ?
            (int)InterfaceLogic::settingsLogic.selectedAction : -1;
          const int highlightedRow =
            InterfaceLogic::settingsLogic.highlightedControl == SettingsLogic::ctrlKeyBindTable ?
            (int)InterfaceLogic::settingsLogic.highlightedAction : -1;

          for (int row = 0, count = keyBindingGridLayout->getRowCount(); row < count; row++)
          {
            float left0 = keyBindingGridLayout->getColumnGlobalLeft(0);
            const float width0 = keyBindingGridLayout->getColumnWidth(0);
            const float left1 = keyBindingGridLayout->getColumnGlobalLeft(1);
            const float width1 = keyBindingGridLayout->getColumnWidth(1);
            const float top = keyBindingGridLayout->getRowGlobalTop(row) + shift;
            const float height = keyBindingGridLayout->getRowHeight(row);
            Binding::Action action = static_cast<Binding::Action>(row);
            const char * actionName = Binding::getActionName(action);
            Key key = Binding::getActionKey(action);
            const char * keyName = Keys::getName(key);
            const glm::vec3 & bkColor =
              row == selectedRow ? Palette::settingsActiveRowBackground :
              row == highlightedRow ? Palette::settingsMouseoverRowBackground :
              Palette::settingsInactiveRowBackground;
            const glm::vec3 & fgColor =
              row == selectedRow ? Palette::settingsActiveRowText :
              row == highlightedRow ? Palette::settingsMouseoverRowText :
              Palette::settingsInactiveRowText;
            buildSmoothRect(left0, top, width0, height, edgeBlurWidth, bkColor, 1.0);
            buildSmoothRect(left1, top, width1, height, edgeBlurWidth, bkColor, 1.0);
            left0 += Layout::settingsPanelRowCaptionIndent;
            buildTextMesh(left0, top, width0, height, actionName, 
                          Layout::settingsPanelRowCaptionHeight, fgColor, 1.0f, 0.0f, haLeft, vaCenter);
            buildTextMesh(left1, top, width1, height, keyName, 
                          Layout::settingsPanelRowCaptionHeight, fgColor, 1.0f, 0.0f, haCenter, vaCenter);
          }
        }
      }

      if (LayoutObject * backButtonLayout = settingsWindowLayout->getChild(loSettingsBackButton))
      {
        for (int column = 0, count = backButtonLayout->getColumnCount(); column < count; column++)
        {
          const float shevronLeft = backButtonLayout->getColumnGlobalLeft(column);
          const float shevronTop = backButtonLayout->getGlobalTop() + shift;
          const float shevronWidth = backButtonLayout->getColumnWidth(column);
          const float shevronHeight = backButtonLayout->height;
          const glm::vec3 & shevronColor = InterfaceLogic::settingsLogic.backButtonHighlighted ? 
                                           Palette::settingsBackButtonHighlighted : 
                                           Palette::settingsBackButton;
          buildTexturedRect(shevronLeft, shevronTop, shevronWidth, shevronHeight, 
                            tiBackShevron, shevronColor, 1.0f);
        }
      }
    }
  }
}


void OpenGLRender::buildLeaderboardWindow()
{
  if (LayoutObject * leaderboardLayout = Layout::screen.getChild(loLeaderboard))
  {
    if (LayoutObject * leaderboardWindowLayout = leaderboardLayout->getChild(loLeaderboardWindow))
    {
      const float opProgress = 1.0f - InterfaceLogic::leaderboardLogic.transitionProgress;
      const float sqOpProgress = opProgress * opProgress;
      const float fullLeaderboardHeight = 
        leaderboardWindowLayout->height + 2.0f * Layout::leaderboardGlowWidth;
      const float shift = 
        -sqOpProgress * (fullLeaderboardHeight + leaderboardWindowLayout->getGlobalTop());
      const float left = leaderboardWindowLayout->getGlobalLeft();
      const float top = leaderboardWindowLayout->getGlobalTop() + shift;
      const float width = leaderboardWindowLayout->width;
      const float height = leaderboardWindowLayout->height;
      buildWindow(left, top, width, height, Layout::leaderboardCornerSize, 
                  Layout::leaderboardGlowWidth, Palette::leaderboardBackgroundTop, 
                  Palette::leaderboardBackgroundBottom, Palette::leaderboardGlow);

      if (LayoutObject * settingTitleShadowLayout = leaderboardWindowLayout->getChild(loLeaderboardTitleShadow))
      {
        const float left = settingTitleShadowLayout->getGlobalLeft();
        const float top = settingTitleShadowLayout->getGlobalTop() + shift;
        const float width = settingTitleShadowLayout->width;
        const float height = settingTitleShadowLayout->height;
        buildTextMesh(left, top, width, height, "LEADERBOARD", Layout::leaderboardTitleHeight, 
                      glm::vec3(0.0f), Palette::leaderboardTitleShadowAlpha, 
                      Palette::leaderboardTitleShadowBlur, haLeft, vaTop);
      }

      if (LayoutObject * settingTitleLayout = leaderboardWindowLayout->getChild(loLeaderboardTitle))
      {
        const float left = settingTitleLayout->getGlobalLeft();
        const float top = settingTitleLayout->getGlobalTop() + shift;
        const float width = settingTitleLayout->width;
        const float height = settingTitleLayout->height;
        buildTextMesh(left, top, width, height, "LEADERBOARD", Layout::leaderboardTitleHeight, 
                      Palette::leaderboardTitleText, 1.0f, 0.0f, haLeft, vaTop);
      }

      if (LayoutObject * settingPanelLayout = leaderboardWindowLayout->getChild(loLeaderboardPanel))
      {
        const float left = settingPanelLayout->getGlobalLeft();
        const float top = settingPanelLayout->getGlobalTop() + shift;
        const float width = settingPanelLayout->width;
        const float height = settingPanelLayout->height;
        buildVertGradientRect(left, top, width, height, 
                              Palette::leaderboardPanelBackgroundTop, 1.0f, 
                              Palette::leaderboardPanelBackgroundBottom, 1.0f);
        buildFrameRect(left, top, width, height, 
                       Layout::leaderboardPanelBorderWidth, Palette::leaderboardPanelBorder, 1.0f);

        const float headerTop = settingPanelLayout->getRowGlobalTop(0) + shift;
        const float headerHeight = settingPanelLayout->getRowHeight(0);

        const float placeColLeft = settingPanelLayout->getColumnGlobalLeft(0);
        const float placeColWidth = settingPanelLayout->getColumnWidth(0);
        const float nameColLeft = settingPanelLayout->getColumnGlobalLeft(1);
        const float nameColWidth = settingPanelLayout->getColumnWidth(1);
        const float levelColLeft = settingPanelLayout->getColumnGlobalLeft(2);
        const float levelColWidth = settingPanelLayout->getColumnWidth(2);
        const float scoreColLeft = settingPanelLayout->getColumnGlobalLeft(3);
        const float scoreColWidth = settingPanelLayout->getColumnWidth(3);

        const glm::vec3 headerColor = Palette::leaderboardPanelHeaderText;

        buildTextMesh(nameColLeft + Layout::leaderboardPanelNameLeftIndent, headerTop, 
                      nameColWidth, headerHeight, "NAME", Layout::leaderboardPanelHeaderTextHeight, 
                      headerColor, 1.0f, 0.0f, haLeft, vaCenter);
        buildTextMesh(levelColLeft, headerTop, levelColWidth, headerHeight, "LEVEL", 
                      Layout::leaderboardPanelHeaderTextHeight, 
                      headerColor, 1.0f, 0.0f, haCenter, vaCenter);
        buildTextMesh(scoreColLeft, headerTop, scoreColWidth - Layout::leaderboardPanelScoreRightIndent, 
                      headerHeight, "SCORE", Layout::leaderboardPanelHeaderTextHeight, 
                      headerColor, 1.0f, 0.0f, haRight, vaCenter);

        const int leadersCount = InterfaceLogic::leaderboardLogic.leadersCount;

        for (int i = 0; i < leadersCount; i++)
        {
          const LeaderboardLogic::LeaderData & leader = InterfaceLogic::leaderboardLogic.getLeaderData(i);
          const int editRow = InterfaceLogic::leaderboardLogic.editRow;
          const glm::vec3 textColor = (i == editRow) ? 
                                      Palette::leaderboardEditRowText : 
                                      Palette::leaderboardRowText;
          const float rowTop = settingPanelLayout->getRowGlobalTop(i + 1) + shift;
          const float rowHeight = settingPanelLayout->getRowHeight(i + 1);
          char strbuf[32];
          itoa(i + 1, strbuf, 10);
          buildTextMesh(placeColLeft, rowTop, placeColWidth, rowHeight, strbuf, 
                        Layout::leaderboardPanelRowTextHeight, textColor, 1.0f, 0.0f, haCenter, vaCenter);
          float nameWidth = buildTextMesh(nameColLeft + Layout::leaderboardPanelNameLeftIndent, rowTop, 
                                          nameColWidth, rowHeight, leader.name, 
                                          Layout::leaderboardPanelRowTextHeight,
                                          textColor, 1.0f, 0.0f, haLeft, vaCenter);

          if (i == editRow && (Time::counter % Time::freq > Time::freq / 2))
          {
            const float cursorHeight = 0.9f * Layout::leaderboardPanelRowTextHeight;
            const float cursorWidth = 0.5f * cursorHeight;
            const float cursorLeft = nameColLeft + Layout::leaderboardPanelNameLeftIndent + nameWidth - 
                                     rowHeight * font.falloff();
            const float cursorTop = rowTop + 0.5f * (rowHeight - cursorHeight);
            buildSmoothRect(cursorLeft, cursorTop, cursorWidth, cursorHeight, 0.1f * cursorWidth, 
                            textColor, 1.0f);
          }

          itoa(leader.level, strbuf, 10);
          buildTextMesh(levelColLeft, rowTop, levelColWidth, rowHeight, strbuf,
                        Layout::leaderboardPanelRowTextHeight, textColor, 1.0f, 0.0f, haCenter, vaCenter);
          itoa(leader.score, strbuf, 10);
          buildTextMesh(scoreColLeft, rowTop, scoreColWidth - Layout::leaderboardPanelScoreRightIndent, 
                        rowHeight, strbuf, Layout::leaderboardPanelRowTextHeight, textColor, 
                        1.0f, 0.0f, haRight, vaCenter);
        }
      }

      if (LayoutObject * backButtonLayout = leaderboardWindowLayout->getChild(loLeaderboardBackButton))
      {
        for (int column = 0, count = backButtonLayout->getColumnCount(); column < count; column++)
        {
          const float shevronLeft = backButtonLayout->getColumnGlobalLeft(column);
          const float shevronTop = backButtonLayout->getGlobalTop() + shift;
          const float shevronWidth = backButtonLayout->getColumnWidth(column);
          const float shevronHeight = backButtonLayout->height;
          const glm::vec3 & shevronColor = InterfaceLogic::leaderboardLogic.backButtonHighlighted ? 
                                           Palette::leaderboardBackButtonHighlighted : 
                                           Palette::leaderboardBackButton;
          buildTexturedRect(shevronLeft, shevronTop, shevronWidth, shevronHeight,
                            tiBackShevron, shevronColor, 1.0f);
        }
      }
    }
  }
}


void OpenGLRender::buildCountdown()
{
  if (LayoutObject * fieldLayout = Layout::screen.getChildRecursive(loField))
  {
    const float xpos = fieldLayout->getGlobalLeft() + 0.5f * fieldLayout->width;
    const float ypos = fieldLayout->getGlobalTop() + 0.5f * fieldLayout->height;
    const int num = (int)GameLogic::countdownTimeLeft;
    const float numProgress = GameLogic::countdownTimeLeft - num;
    char numStr[32];
    itoa(num, numStr, 10);
    char * text = num ? numStr : "GO";
    const float animInTime = 0.25f;
    const float animOutTime = 0.25f;
    const float inScale = 1.0f - glm::clamp(1.0f - numProgress, 0.0f, animInTime) / animInTime;
    const float outScale = 
      1.0f - (num ? glm::clamp(numProgress, 0.0f, animOutTime) / animOutTime : 1.0f);
    const float scale = (1.0f - inScale * inScale) * (1.0f - outScale * outScale);
    const float progressParam = 2.0f * (0.5f - numProgress);
    const float dy = -0.2f + (num || numProgress > 0.5f ? 0.1f : 0.65f) * 
      progressParam * progressParam * progressParam;
    const float textHeight = 0.1f;

    if (ypos + dy + 0.5f * textHeight < fieldLayout->getGlobalTop() + fieldLayout->height)
      buildTextMesh(xpos, ypos + dy, 0.0f, 0.0f, text, textHeight * scale, glm::vec3(1.0f), 
                    0.0f, 0.0f, haCenter, vaCenter);
  }
}


void OpenGLRender::buildLevelUp()
{
  if (LayoutObject * fieldLayout = Layout::screen.getChildRecursive(loField))
  {
    const float effectTime = 2.0f;
    const float magnifTime = 0.5f;
    static float effectTimeLeft = 0.0f;
    static int curLevel = GameLogic::curLevel;

    if (GameLogic::curLevel != 1 && GameLogic::curLevel != curLevel)
      effectTimeLeft = effectTime;

    if (effectTimeLeft > 0.0f)
    {
      const float xpos = fieldLayout->getGlobalLeft() + 0.5f * fieldLayout->width;
      const float ypos = fieldLayout->getGlobalTop() + 0.5f * fieldLayout->height;

      const float textHeight = 0.075f;
      float scale = glm::clamp((effectTime - effectTimeLeft) / magnifTime, 0.0f, 1.0f);
      float progress = (effectTime - effectTimeLeft) / effectTime;
      const float yOffs = 0.2f;
      float dy = -yOffs + yOffs * (3.0f * progress * progress - 5.0f * pow(progress, 5.0f));

      if (ypos + dy > fieldLayout->getGlobalTop())
      {
        const char * levelUpText = "LEVEL UP";
        buildTextMesh(xpos, ypos + dy, 0.0f, 0.0f, levelUpText, textHeight * scale, 
                      glm::vec3(0.0f), 0.75f, 1.0f, haCenter, vaTop);
        buildTextMesh(xpos, ypos + dy, 0.0f, 0.0f, levelUpText, textHeight * scale, 
                      glm::vec3(1.0f), 0.0f, 0.0f, haCenter, vaTop);
      }
      else
        effectTimeLeft = -1.0f;
    }

    effectTimeLeft -= Time::timerDelta;
    curLevel = GameLogic::curLevel;
  }
}


void OpenGLRender::buildDropPredictor()
{
  if (LayoutObject * fieldLayout = Layout::screen.getChildRecursive(loField))
  {
    const float fieldLeft = fieldLayout->getGlobalLeft();
    const float fieldTop = fieldLayout->getGlobalTop();
    const float cellSize = fieldLayout->width / Field::width;
    const glm::vec3 figureColor = Palette::cellColorArray[GameLogic::curFigure.color];
    const int fieldWidth = Field::width;
    const int fieldHeight = Field::height;
    int dim = GameLogic::curFigure.dim;
    int yArray[Figure::dimMax];

    for (int x = 0; x < dim; x++)
    {
      yArray[x] = 0;

      for (int y = dim - 1; y >= 0; y--)
      {
        if (!GameLogic::curFigure.getCell(x, y)->isEmpty())
        {
          int fieldX = GameLogic::curFigureX + x;
          int fieldY = GameLogic::curFigureY + y + 1;

          while (fieldY < fieldHeight && GameLogic::field.getCell(fieldX, fieldY)->isEmpty())
            fieldY++;

          if (fieldY > GameLogic::curFigureY + y + 1)
            yArray[x] = fieldY;

          break;
        }
      }
    }

    static const float glowMinHeight = 0.3f;
    static const float glowMaxHeight = 0.6f;
    static const float glowMinSpeed = 1.0f;
    static const float glowMaxSpeed = 2.0f;
    static const float glowHeightRange = glowMaxHeight - glowMinHeight;
    static const float glowSpeedRange = glowMaxSpeed - glowMinSpeed;

    struct FlameJitter
    {
      float currentHeight;
      float targetHeight;
      float speed;

      FlameJitter()
      {
        currentHeight = glowMinHeight + glowHeightRange * float(rand()) / RAND_MAX;
        targetHeight = glowMinHeight + glowHeightRange * float(rand()) / RAND_MAX;
        speed = glowMinSpeed + glowSpeedRange * float(rand()) / RAND_MAX;
      }
    };

    static FlameJitter jitter[Figure::dimMax + 1];

    static const int partCount = 50;
    static const float partMinLife = 0.75f;
    static const float partMaxLife = 1.5f;
    static const float partMinSpeed = 1.0f;
    static const float partMaxSpeed = 1.5f;
    static const float partMinSize = 0.06f;
    static const float partMaxSize = 0.1f;
    static const float partMinAlpha = 0.5f;
    static const float partMaxAlpha = 0.75f;
    static const float partLifeRange = partMaxLife - partMinLife;
    static const float partSpeedRange = partMaxSpeed - partMinSpeed;
    static const float partSizeRange = partMaxSize - partMinSize;
    static const float partAlphaRange = partMaxAlpha - partMinAlpha;

    struct CellParticles
    {
      float timeLeft[partCount];
      float lifeTime[partCount];
      float dx[partCount];
      float speed[partCount];
      float size[partCount];
      float alpha[partCount];

      CellParticles()
      {
        for (int i = 0; i < partCount; i++)
        {
          lifeTime[i] = partMinLife + partLifeRange * float(rand()) / RAND_MAX;
          timeLeft[i] = lifeTime[i] * float(rand()) / RAND_MAX;
          dx[i] = float(rand()) / RAND_MAX;
          speed[i] = partMinSpeed + partSpeedRange * float(rand()) / RAND_MAX;
          size[i] = partMinSize + partSizeRange * float(rand()) / RAND_MAX;
          alpha[i] = partMinAlpha + partAlphaRange * float(rand()) / RAND_MAX;
        }
      }
    };

    static CellParticles particles[Figure::dimMax];

    for (int x = 0; x < dim + 1; x++)
    {
      float & currentHeight = jitter[x].currentHeight;
      float & targetHeight = jitter[x].targetHeight;
      float & speed = jitter[x].speed;

      float dh = targetHeight - currentHeight;
      float sign = (dh >= 0.0f) ? 1.0f : -1.0f;
      currentHeight = glm::clamp(currentHeight + sign * speed * Time::timerDelta, 
                                 glowMinHeight, glowMaxHeight);

      if (sign * (targetHeight - currentHeight) <= 0.0f)
      {
        targetHeight = glowMinHeight + glowHeightRange * float(rand()) / RAND_MAX;
        speed = glowMinSpeed + glowSpeedRange * float(rand()) / RAND_MAX;
      }
    }

    for (int x = 0; x < dim; x++)
    {
      int fieldX = GameLogic::curFigureX + x;
      int fieldY = yArray[x];

      if (fieldY)
      {
        bool leftEmpty = !x || fieldY != yArray[(x - 1)];
        bool rightEmpty = (x == dim - 1) || fieldY != yArray[(x + 1)];
        float rowElevation = (fieldY < fieldHeight) ? GameLogic::getRowCurrentElevation(fieldY) : 0.0f;

        // flame effect
        float & currentHeight0 = jitter[x].currentHeight;
        float & currentHeight1 = jitter[x + 1].currentHeight;

        float left = fieldLeft + cellSize * fieldX;
        float top0 = fieldTop + cellSize * (fieldY - rowElevation - currentHeight0);
        float top1 = fieldTop + cellSize * (fieldY - rowElevation - currentHeight1);

        float width = cellSize;
        float height0 = cellSize * currentHeight0;
        float height1 = cellSize * currentHeight1;
        float top05 = 0.5f * (top0 + top1);
        float height05 = 0.5f * (height0 + height1);

        if (fieldY < fieldHeight)
        {
          const float innerOffset = cellSize * 2.0f / atlasSpriteSize;
          top0 += innerOffset;
          top1 += innerOffset;
          top05 += innerOffset;
        }

        const int vertCnt = 6;

        glm::vec2 verts[vertCnt] =
        {
          { left,                 top0 },
          { left,                 top0 + height0 },
          { left + 0.5f * width,  top05 },
          { left + 0.5f * width,  top05 + height05 },
          { left + width,         top1 },
          { left + width,         top1 + height1 },
        };

        float uvLeft = leftEmpty ? 0.0f : 0.5f;
        float uvRight = rightEmpty ? 1.0f : 0.5f;

        glm::vec2 uv[vertCnt] =
        {
          { uvLeft,   0.0f },
          { uvLeft,   1.0f },
          { 0.5f,     0.0f },
          { 0.5f,     1.0f },
          { uvRight,  0.0f },
          { uvRight,  1.0f },
        };

        const glm::vec3 color = 0.05f + glm::vec3(0.4f) * figureColor;

        for (int vi = 0; vi < vertCnt - 2; vi++)
        {
          addAtlasVertex(verts[vi + 0], uv[vi + 0], tiFlame, color, 0.0f);
          addAtlasVertex(verts[vi + 1], uv[vi + 1], tiFlame, color, 0.0f);
          addAtlasVertex(verts[vi + 2], uv[vi + 2], tiFlame, color, 0.0f);
        }

        // particles
        CellParticles & cellParticles = particles[x];

        for (int i = 0; i < partCount; i++)
        {
          float & lifeTime = cellParticles.lifeTime[i];
          float & timeLeft = cellParticles.timeLeft[i];
          float & dx = cellParticles.dx[i];
          float & speed = cellParticles.speed[i];
          float & size = cellParticles.size[i];
          float & alpha = cellParticles.alpha[i];

          float timePassed = lifeTime - timeLeft;
          float sqTimePassed = timePassed * timePassed;
          float leftSpeedCorr = leftEmpty ? 
                                0.25f + pow(1.0f - glm::max(2.0f * (0.5f - dx), 0.0f), 2.0f) : 
                                1.0f;
          float rightSpeedCorr = rightEmpty ? 
                                 0.25f + pow(1.0f - glm::max(2.0f * (dx - 0.5f), 0.0f), 2.0f) : 
                                 1.0f;
          float curSpeed = leftSpeedCorr * rightSpeedCorr * speed;
          float dy = curSpeed * sqTimePassed - rowElevation;
          float leftAlphaCorr = leftEmpty ? 1.0f - glm::max(2.0f * (0.5f - dx), 0.0f) : 1.0f;
          float rightAlphaCorr = rightEmpty ? 1.0f - glm::max(2.0f * (dx - 0.5f), 0.0f) : 1.0f; 
          float nominalAlpha = glm::clamp(alpha - (1.0f - timeLeft * timeLeft / (lifeTime * lifeTime)), 
                                          0.0f, 1.0f);
          float curAlpha = leftAlphaCorr * rightAlphaCorr * nominalAlpha;
          float left = fieldLeft + cellSize * (fieldX + dx - 0.5f * size);
          float top = fieldTop + cellSize * (fieldY - dy - size);

          if (fieldY < fieldHeight)
          {
            const float innerOffset = cellSize * 2.0f / atlasSpriteSize;
            top += innerOffset;
          }

          buildTexturedRect(left, top, cellSize * size, cellSize * size, tiDropSparkle, 
                            figureColor * curAlpha, 0.0f);
          timeLeft -= Time::timerDelta;

          if (timeLeft < 0.0f)
          {
            lifeTime = partMinLife + partLifeRange * float(rand()) / RAND_MAX;

            while (timeLeft < 0.0f) 
              timeLeft += lifeTime;

            dx = float(rand()) / RAND_MAX;
            speed = partMinSpeed + partSpeedRange * float(rand()) / RAND_MAX;
            size = partMinSize + partSizeRange * float(rand()) / RAND_MAX;
            alpha = partMinAlpha + partAlphaRange * float(rand()) / RAND_MAX;
          }
        }
      }
    }
  }
}


void OpenGLRender::updateGameLayer()
{
  clearVertices();
  buildBackground();

  if (GameLogic::state == GameLogic::stCountdown)
    buildCountdown();

  if (GameLogic::state == GameLogic::stPlaying ||
    GameLogic::state == GameLogic::stPaused || 
    GameLogic::state == GameLogic::stGameOver)
  {
    buidField();
    buildHoldFigure();
    buildNextFigures();
    buildDropTrails();
    buildRowFlashes();
    buildLevelUp();
    buildDropPredictor();
  }

  drawMesh();

  const float gameOverTime = (float)GameLogic::gameOverTime;
  const float gameOverInTime = 0.5f;
  const float gameOverOutTime = 0.5f;
  const float gameOverTimeLeft = GameLogic::gameOverTimeLeft;

  if (gameOverTimeLeft > 0.0f)
  {
    const float inProgress = glm::clamp((gameOverTime - gameOverTimeLeft) / gameOverInTime, 0.0f, 1.0f);
    const float progress = glm::clamp((gameOverTime - gameOverTimeLeft) / gameOverTime, 0.0f, 1.0f);
    const float outProgress = glm::clamp(gameOverTimeLeft / gameOverOutTime, 0.0f, 1.0f);
    const float opPowInProgress = 1.0f - pow(1.0f - inProgress, 3.0f);
    const float opPowOutProgress = 1.0f - pow(1.0f - outProgress, 3.0f);

    clearVertices();
    buildRect(0.0f, 0.0f, 1.0f, 1.0f, glm::vec3(0.0f), Palette::backgroundShadeAlpha * opPowInProgress);
    drawMesh();

    if (LayoutObject * gameLayout = Layout::screen.getChild(loGame))
    {
      const float xpos = gameLayout->getGlobalLeft() + 0.5f * gameLayout->width;
      const float ypos = gameLayout->getGlobalTop() + 0.5f * gameLayout->height;

      const float textHeight = 0.075f;
      const float scale = opPowInProgress / glm::clamp(opPowOutProgress, 0.0001f, 1.0f);
      const float blur = 1.0f - opPowOutProgress;
      const char * levelUpText = "GAME OVER";
      clearVertices();
      buildTextMesh(xpos, ypos, 0.0f, 0.0f, levelUpText, textHeight * scale, 
                    opPowOutProgress * glm::vec3(0.0f), 1.0f, 1.0f, haCenter, vaCenter);
      buildTextMesh(xpos, ypos, 0.0f, 0.0f, levelUpText, textHeight * scale, 
                    opPowOutProgress * glm::vec3(1.0f), 0.0f, blur, haCenter, vaCenter);
      drawMesh();
    }
  }
}


void OpenGLRender::updateSettingsLayer()
{
  if (InterfaceLogic::state == InterfaceLogic::stSettings)
  {
    clearVertices();
    buildRect(0.0f, 0.0f, 1.0f, 1.0f, glm::vec3(0.0f), Palette::backgroundShadeAlpha);
    drawMesh();

    clearVertices();
    buildSettingsWindow();
    drawMesh();

    static float keyBindBkShade = 0.0f;
    const float keyBindBkShadingSpeed = 5.0f;

    if (InterfaceLogic::settingsLogic.state == SettingsLogic::stKeyWaiting)
    {
      if (keyBindBkShade > 0.0f)
      {
        clearVertices();
        buildRect(0.0f, 0.0f, 1.0f, 1.0f, glm::vec3(0.0f), 
                  Palette::backgroundShadeAlpha * keyBindBkShade);
        drawMesh();
      }

      clearVertices();

      if (LayoutObject * settingsLayout = Layout::screen.getChild(loSettings))
      {
        if (LayoutObject * bindingMsgLayout = settingsLayout->getChild(loBindingMessage))
        {
          const float left = bindingMsgLayout->getGlobalLeft();
          const float top = bindingMsgLayout->getGlobalTop();
          const float width = bindingMsgLayout->width;
          const float height = bindingMsgLayout->height;
          buildRect(left, top, width, height, Palette::settingsBindingMsgBackground, 1.0f);
          buildFrameRect(left, top, width, height, Layout::settingsBindingMsgBorder, 
                         Palette::settingsBindingMsgBorder, 1.0f);
          buildTextMesh(left, top, width, height, "PRESS KEY", Layout::settingsPanelRowCaptionHeight, 
                        Palette::settingsBindingMsgText, 1.0f, 0.0f, haCenter, vaCenter);
        }
      }

      drawMesh();
    }

    float shadeSpeed = (InterfaceLogic::settingsLogic.state == SettingsLogic::stKeyWaiting) ? 
                        keyBindBkShadingSpeed : 
                       -keyBindBkShadingSpeed;
    keyBindBkShade = glm::clamp(keyBindBkShade + shadeSpeed * Time::timerDelta, 0.0f, 1.0f);
  }
}


void OpenGLRender::updateLeaderboardLayer()
{
  if (InterfaceLogic::state == InterfaceLogic::stLeaderboard)
  {
    clearVertices();
    buildRect(0.0f, 0.0f, 1.0f, 1.0f, glm::vec3(0.0f), Palette::backgroundShadeAlpha);
    drawMesh();

    clearVertices();
    buildLeaderboardWindow();
    drawMesh();
  }
}


void OpenGLRender::updateMenuLayer()
{
  MenuLogic * menuLogic = NULL;
  LayoutObject * menuLayout = NULL;
  float shadeProgress = 0.0f;

  switch (InterfaceLogic::state)
  {
  case InterfaceLogic::stMainMenu:
    menuLogic = &InterfaceLogic::mainMenu;
    menuLayout = Layout::screen.getChild(loMainMenu);
    shadeProgress = InterfaceLogic::menuShadeProgress;
    break;

  case InterfaceLogic::stInGameMenu:
    menuLogic = &InterfaceLogic::inGameMenu;
    menuLayout = Layout::screen.getChild(loInGameMenu);
    shadeProgress = InterfaceLogic::menuShadeProgress;
    break;

  case InterfaceLogic::stQuitConfirmation:
    menuLogic = &InterfaceLogic::quitConfirmationMenu;
    menuLayout = Layout::screen.getChild(loQuitConfirmationMenu);
    shadeProgress = InterfaceLogic::menuShadeProgress;
    break;

  case InterfaceLogic::stRestartConfirmation:
    menuLogic = &InterfaceLogic::restartConfirmationMenu;
    menuLayout = Layout::screen.getChild(loRestartConfirmationMenu);
    shadeProgress = InterfaceLogic::menuShadeProgress;
    break;

  case InterfaceLogic::stExitToMainConfirmation:
    menuLogic = &InterfaceLogic::exitToMainConfirmationMenu;
    menuLayout = Layout::screen.getChild(loExitToMainConfirmationMenu);
    shadeProgress = InterfaceLogic::menuShadeProgress;
    break;

  case InterfaceLogic::stSettings: 
    menuLogic = &InterfaceLogic::settingsLogic.saveConfirmationMenu;
    menuLayout = Layout::screen.getChild(loSaveSettingsMenu);
    shadeProgress = InterfaceLogic::settingsLogic.saveConfirmationMenu.transitionProgress;
    break;

  case InterfaceLogic::stLeaderboard: 
    break;
  case InterfaceLogic::stHidden: 
    break;
  default: 
    assert(0);
    break;
  }

  if (menuLogic && menuLayout)
  {
    clearVertices();
    buildRect(0.0f, 0.0f, 1.0f, 1.0f, glm::vec3(0.0f), Palette::backgroundShadeAlpha * shadeProgress);
    drawMesh();

    clearVertices();
    buildMenu(menuLogic, menuLayout);
    drawMesh();
  }
}

