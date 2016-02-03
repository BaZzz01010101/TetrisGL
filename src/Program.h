#pragma once
#include "Shader.h"

class Program
{
private:
  // TODO : forbid copy constructor and operator
  GLint id;
  mutable std::vector<char> errMsg;

public:
  Program();
  ~Program();

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
