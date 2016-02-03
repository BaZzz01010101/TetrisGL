#pragma once

class Shader
{
private:
  // TODO : forbid copy constructor and operator
  GLenum type;
  GLuint id;
  std::vector<char> errMsg;

public:
  Shader(GLenum type);
  virtual ~Shader();

  inline GLuint getId() const { return id; }
  void compileFromString(const char * source);
};

