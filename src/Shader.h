#pragma once
#include "glall.h"

class Shader
{
private:
  GLenum type;
  GLuint id;
  bool errorFlagged;
  std::vector<char> errMsg;
  void setErrMsg(const char * str);
  bool checkGlErrors();

public:
  Shader(GLenum type);
  virtual ~Shader();

  inline GLuint getId() const { return id; }
  inline bool isError() const { return errorFlagged; }
  inline char * getLastError() { return &errMsg.front(); }
  bool compileFromString(const char * source);
  bool compileFromFile(const char * filename);
};

