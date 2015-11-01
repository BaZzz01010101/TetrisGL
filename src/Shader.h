#pragma once

class Shader
{
private:
  GLenum type;
  GLuint id;
  std::vector<char> errMsg;

public:
  Shader(GLenum type);
  virtual ~Shader();

  inline GLuint getId() const { return id; }
  void compileFromString(const char * source);
};

