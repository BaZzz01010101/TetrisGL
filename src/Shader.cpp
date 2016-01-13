#include "static_headers.h"

#include "Shader.h"
#include "Globals.h"

Shader::Shader(GLenum type)
{
  this->type = type;
  id = 0;
}


Shader::~Shader()
{
  // TODO: add checking id and opengl errors
  glDeleteShader(id);
}

void Shader::compileFromString(const char * source)
{
  GLint Result = GL_FALSE;

  if (!id)
  {
    checkGlErrors();
    id = glCreateShader(type);
    assert(!checkGlErrors());
  }

  glShaderSource(id, 1, &source, NULL);
  assert(!checkGlErrors());

  glCompileShader(id);
  assert(!checkGlErrors());

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
