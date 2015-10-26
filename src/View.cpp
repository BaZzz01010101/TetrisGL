#include <assert.h>
#include <math.h>

#include "View.h"
#include "Crosy.h"
#include "Globals.h"
#include "3rdParty/SOIL2/SOIL2.h"

View::View(Model & model) :
  figureVert(GL_VERTEX_SHADER),
  figureFrag(GL_FRAGMENT_SHADER),
  model(model),
  mainMesh(model)
{
}


View::~View()
{
}

void View::init(int winWidth, int winHeight)
{
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

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_LOD_BIAS, -1);
  assert(!checkGlErrors());

  glDisable(GL_CULL_FACE);
  assert(!checkGlErrors());

  glEnable(GL_DEPTH_TEST);
  assert(!checkGlErrors());

  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  assert(!checkGlErrors());

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  assert(!checkGlErrors());

  mainMesh.init();

  resize(winWidth, winHeight);
}

void View::resize(int width, int height)
{
  const float gameAspect = Globals::gameBkSize.x / Globals::gameBkSize.y;

  if (float(width) / height > gameAspect)
    glViewport((width - height) / 2, 0, height, height);
  else
    glViewport(int(width * (1.0f - 1.0f / gameAspect)) / 2, (height - int(width / gameAspect)) / 2, int(width / gameAspect), int(width / gameAspect));

  mainMesh.fillDepthBuffer();
}

void View::update()
{
  if (model.showWireframe)
  {
    glDisable(GL_BLEND);
    assert(!checkGlErrors());
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    assert(!checkGlErrors());
  }
  else
  {
    glEnable(GL_BLEND);
    assert(!checkGlErrors());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(!checkGlErrors());
  }

  if (model.glassChanged)
  {
    mainMesh.rebuild();
//    model.glassChanged = false;
  }

  glClear(GL_COLOR_BUFFER_BIT);
  assert(!checkGlErrors());

  mainMesh.draw();
}

