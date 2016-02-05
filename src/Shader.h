#pragma once

class Shader
{
private:
  GLenum type;
  GLuint id;
  std::vector<char> errMsg;
  Shader & operator=(const Shader &);
  Shader(const Shader &);

public:
  Shader(GLenum type);

  void init();
  void quit();
  inline GLuint getId() const { return id; }
  void compileFromString(const char * source);
};

