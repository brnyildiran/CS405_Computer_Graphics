#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <GL/glew.h>
#include "shader.h"

Shader::Shader() {}

Shader::Shader(const char *vertexFilePath, const char *fragmentFilePath) {
  GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  std::string vertexShaderCode;
  std::ifstream vertexShaderStream(vertexFilePath, std::ios::in);
  if (vertexShaderStream.is_open()) {
    std::stringstream sstr;
    sstr << vertexShaderStream.rdbuf();
    vertexShaderCode = sstr.str();
    vertexShaderStream.close();
  }
  else {
    std::cout << "An error occured while opening " << vertexFilePath << std::endl;
    exit(1);
  }
  std::string fragmentShaderCode;
  std::ifstream fragmentShaderStream(fragmentFilePath, std::ios::in);
  if (fragmentShaderStream.is_open()) {
    std::stringstream sstr;
    sstr << fragmentShaderStream.rdbuf();
    fragmentShaderCode = sstr.str();
    fragmentShaderStream.close();
  }
  else {
    std::cout << "An error occured while opening " << fragmentFilePath << std::endl;
    exit(1);
  }
  GLint result = GL_FALSE;
  int infoLogLength;
  std::cout << "Compiling shader \"" << vertexFilePath << "\"" << std::endl;
  char const *vertexSourcePtr = vertexShaderCode.c_str();
  glShaderSource(vertexShaderId, 1, &vertexSourcePtr, NULL);
  glCompileShader(vertexShaderId);
  glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &result);
  glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 0) {
    std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
    glGetShaderInfoLog(vertexShaderId, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
    std::cout << &vertexShaderErrorMessage[0] << std::endl;
  }
  std::cout << "Compiling shader \"" << fragmentFilePath << "\"" << std::endl;
  char const *fragmentSourcePtr = fragmentShaderCode.c_str();
  glShaderSource(fragmentShaderId, 1, &fragmentSourcePtr, NULL);
  glCompileShader(fragmentShaderId);
  glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &result);
  glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 0) {
    std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
    glGetShaderInfoLog(fragmentShaderId, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
    std::cout << &fragmentShaderErrorMessage[0] << std::endl;
  }
  std::cout << "Linking program" << std::endl;
  id = glCreateProgram();
  glAttachShader(id, vertexShaderId);
  glAttachShader(id, fragmentShaderId);
  glLinkProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &result);
  glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 0) {
    std::vector<char> programErrorMessage(infoLogLength + 1);
    glGetProgramInfoLog(id, infoLogLength, NULL, &programErrorMessage[0]);
    std::cout << &programErrorMessage[0] << std::endl;
  }
  glDetachShader(id, vertexShaderId);
  glDetachShader(id, fragmentShaderId);
  glDeleteShader(vertexShaderId);
  glDeleteShader(fragmentShaderId);
}

void Shader::activate() {
  glUseProgram(id);
}

void Shader::deactivate() {
  glUseProgram(0);
}

void Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
  glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string &name, float x, float y) const {
  glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
  glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const {
  glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
  glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w) {
  glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
  glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
  glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
  glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}