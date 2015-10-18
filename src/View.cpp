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
  glassView(model)
{
}


View::~View()
{
}

void View::init()
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

  glassView.init();

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

  glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
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
}

void View::update()
{
  if (model.glassChanged)
  {
    glassView.rebuildMesh();
//    model.glassChanged = false;
  }

  glassView.draw();
}

