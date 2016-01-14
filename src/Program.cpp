#include "static_headers.h"

#include "Program.h"
#include "Globals.h"

Program::Program() :
  errMsg(1, '\0')
{
  id = 0;
}

Program::~Program()
{
  // TODO: add checking opengl errors
  if (id)
  {
    glDeleteProgram(id);
    //assert(!checkGlErrors());
    GLint curId = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &curId);
    //assert(!checkGlErrors());

    if (curId && curId == id)
    {
      glUseProgram(0);
      //assert(!checkGlErrors());
    }
  }
}

void Program::attachShader(const Shader & shader)
{
  if (!id)
  {
    id = glCreateProgram();
    assert(!checkGlErrors());
  }

  glAttachShader(id, shader.getId());
  assert(!checkGlErrors());
}

void Program::link()
{
  assert(id);

  glLinkProgram(id);
  assert(!checkGlErrors());

  GLint Result = GL_FALSE;
  glGetProgramiv(id, GL_LINK_STATUS, &Result);
  assert(!checkGlErrors());

  if (Result == GL_FALSE)
  {
    int length = 0;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
    assert(!checkGlErrors());

    errMsg.resize(length);
    glGetProgramInfoLog(id, length, NULL, &errMsg.front());
    assert(!checkGlErrors());

    Globals::glErrorMessage = &errMsg.front();
    std::cout << Globals::glErrorMessage << "\n";
    assert(0);
  }
}

void Program::use() const
{
  glUseProgram(id);
  assert(!checkGlErrors());
}

void Program::setUniform(const char * name, GLint value)
{
  GLint uid = glGetUniformLocation(id, name);
  assert(!checkGlErrors());
  assert(uid >= 0);

  if (uid >= 0)
  {
    glUniform1i(uid, value);
    assert(!checkGlErrors());
  }
}

void Program::setUniform(const char * name, GLfloat value)
{
  GLint uid = glGetUniformLocation(id, name);
  assert(!checkGlErrors());
  assert(uid >= 0);

  if (uid >= 0)
  {
    glUniform1f(uid, value);
    assert(!checkGlErrors());
  }
}

void Program::setUniform(const char * name, GLfloat value1, GLfloat value2)
{
  GLint uid = glGetUniformLocation(id, name);
  assert(!checkGlErrors());
  assert(uid >= 0);

  if (uid >= 0)
  {
    glUniform2f(uid, value1, value2);
    assert(!checkGlErrors());
  }
}

void Program::setUniform(const char * name, const glm::vec2 & value)
{
  GLint uid = glGetUniformLocation(id, name);
  assert(!checkGlErrors());
  assert(uid >= 0);

  if (uid >= 0)
  {
    glUniform2fv(uid, 1, &value.x);
    assert(!checkGlErrors());
  }
}

void Program::setUniform(const char * name, const glm::mat3 & value)
{
  GLint uid = glGetUniformLocation(id, name);
  assert(!checkGlErrors());
  assert(uid >= 0);

  if (uid >= 0)
  {
    glUniformMatrix3fv(uid, 1, false, &value[0][0]);
    assert(!checkGlErrors());
  }
}

void Program::bindAttribLocation(GLuint pos, const char * name)
{
  glBindAttribLocation(id, pos, name);
  assert(!checkGlErrors());
}
