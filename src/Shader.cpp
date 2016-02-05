#include "static_headers.h"

#include "Shader.h"
#include "Globals.h"

Shader::Shader(GLenum type)
{
  this->type = type;
  id = 0;
}


void Shader::init()
{
  assert(!id);

  if (!id)
  {
    id = glCreateShader(type);
    assert(!checkGlErrors());
  }
}


void Shader::quit()
{
  assert(id);

  if (id)
  {
    glDeleteShader(id);
    assert(!checkGlErrors());
  }
}


void Shader::compileFromString(const char * source)
{
  assert(id);

  glShaderSource(id, 1, &source, NULL);
  assert(!checkGlErrors());

  glCompileShader(id);
  assert(!checkGlErrors());

  GLint Result = GL_FALSE;
  glGetShaderiv(id, GL_COMPILE_STATUS, &Result);
  assert(!checkGlErrors());

  if (Result == GL_FALSE)
  {
    int length = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    assert(!checkGlErrors());

    errMsg.resize(length);
    glGetShaderInfoLog(id, length, NULL, &errMsg.front());
    assert(!checkGlErrors());

    Globals::glErrorMessage = &errMsg.front();
    std::cout << Globals::glErrorMessage << "\n";
    assert(0);
  }
}
