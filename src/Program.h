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
};
