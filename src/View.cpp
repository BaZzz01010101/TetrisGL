#include "static_headers.h"

#include "View.h"
#include "Crosy.h"
#include "Globals.h"

View::View(Model & model) :
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

  GLenum glewInitResult = glewInit();
  assert(glewInitResult == GLEW_OK);

  // workaround GLEW issue with GL_INVALID_ENUM rising just after glewInit
  glGetError();

  glGenVertexArrays(1, &vaoId);
  assert(!checkGlErrors());

  glBindVertexArray(vaoId);
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

  if (!height || float(width) / height > gameAspect)
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

