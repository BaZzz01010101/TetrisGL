#pragma once
#include "Shader.h"

class Program
{
private:
  GLint id;
  mutable std::vector<char> errMsg;
  Program & operator=(const Program &);
  Program(const Program &);

public:
  Program();

  void init();
  void quit();
  void attachShader(const Shader & shader);
  void link();
  void use() const;
  void setUniform(const char * name, GLint value);
  void setUniform(const char * name, GLfloat value);
  void setUniform(const char * name, GLfloat value1, GLfloat value2);
  void setUniform(const char * name, const glm::vec2 & value);
  void setUniform(const char * name, const glm::mat3 & value);
  void bindAttribLocation(GLuint pos, const char * name);
};
