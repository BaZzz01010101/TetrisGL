#include "Program.h"

Program::Program() :
  errMsg(1, '\0')
{
  id = 0;
  errorFlagged = false;
}

Program::~Program()
{
  if (id)
  {
    glDeleteProgram(id);
    GLint curId = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &curId);

    if (curId && curId == id)
      glUseProgram(0);
  }
}

void Program::setErrMsg(const char * str) const
{
  size_t len = strlen(str);
  errMsg.resize(len + 1);
  strncpy(&errMsg.front(), str, errMsg.size());
  errMsg[len] = '\0';
}

bool Program::checkGlErrors() const
{
  GLenum errCode = glGetError();

  if (errCode)
  {
    const GLubyte * errStr = gluErrorString(errCode);

    if (errStr)
      setErrMsg((const char*)errStr);
    else
      setErrMsg("Unknown error");
  }
  else
    setErrMsg("");

  errorFlagged = errCode != 0;
  return errorFlagged;
}

bool Program::attachShader(const Shader & shader)
{
  if (!id)
  {
    id = glCreateProgram();

    if (checkGlErrors())
      return false;
  }

  glAttachShader(id, shader.getId());

  if (checkGlErrors())
    return false;

  return true;
}

bool Program::link()
{
  GLint Result = GL_FALSE;

  if (!id)
  {
    setErrMsg("No shaders to link");
    return false;
  }

  glLinkProgram(id);

  if (checkGlErrors())
    return false;

  glGetProgramiv(id, GL_LINK_STATUS, &Result);

  if (checkGlErrors())
    return false;

  if (Result == GL_FALSE)
  {
    int length = 0;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);

    if (checkGlErrors())
      return false;
    
    errMsg.resize(length);
    glGetProgramInfoLog(id, length, NULL, &errMsg.front());

    checkGlErrors();

    errorFlagged = true;
    return false;
  }

  errorFlagged = false;
  return true;
}

bool Program::use() const
{
  glUseProgram(id);

  return (!checkGlErrors());
}
