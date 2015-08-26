#pragma once
#include "glall.h"
#include "Shader.h"

class Program
{
private:
  GLint id;
  mutable bool errorFlagged;
  mutable std::vector<char> errMsg;
  void setErrMsg(const char * str) const;
  bool checkGlErrors() const;

public:
  Program();
  ~Program();

  inline GLint getId() const { return id; }
  inline bool isError() const { return errorFlagged; }
  inline char * getLastError() { return &errMsg.front(); }
  bool attachShader(const Shader & shader);
  bool link();
  bool use() const;
  bool setUniform(const char * name, GLint value);
  bool setUniform(const char * name, GLfloat value);
  bool setUniform(const char * name, GLfloat value1, GLfloat value2);
  bool setUniform(const char * name, const glm::vec2 & value);
  bool setUniform(const char * name, const glm::mat3 & value);
};
