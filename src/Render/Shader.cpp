#include "Shader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
  // read shader files
  std::string vertexCode, fragmentCode;
  std::ifstream vFile, fFile;

  vFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    vFile.open(vertexPath);
    fFile.open(fragmentPath);
    std::stringstream vStream, fStream;
    vStream << vFile.rdbuf();
    fStream << fFile.rdbuf();
    vertexCode = vStream.str();
    fragmentCode = fStream.str();
  } catch (std::ifstream::failure &e) {
    std::cerr << "ERROR::SHADER::FILE_NOT_READ: " << e.what() << std::endl;
  }

  const char *vCode = vertexCode.c_str();
  const char *fCode = fragmentCode.c_str();

  // compile shaders
  unsigned int vertex, fragment;

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vCode, nullptr);
  glCompileShader(vertex);
  CheckCompileErrors(vertex, "VERTEX");

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fCode, nullptr);
  glCompileShader(fragment);
  CheckCompileErrors(fragment, "FRAGMENT");

  // link shaders
  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);
  CheckCompileErrors(ID, "PROGRAM");

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

Shader::~Shader() { glDeleteProgram(ID); }

void Shader::Use() { glUseProgram(ID); }

void Shader::SetBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &value) const {
  glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1,
               glm::value_ptr(value));
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &value) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(value));
}

void Shader::CheckCompileErrors(unsigned int shader, const std::string &type) {
  int success;
  char infoLog[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
      std::cerr << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n"
                << infoLog << std::endl;
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
      std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                << infoLog << std::endl;
    }
  }
}
