#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Shader {
public:
  unsigned int ID;

  Shader(const std::string &vertexPath, const std::string &fragmentPath);
  ~Shader();

  void Use();

  // uniforms
  void SetBool(const std::string &name, bool value) const;
  void SetInt(const std::string &name, int value) const;
  void SetFloat(const std::string &name, float value) const;
  void SetVec3(const std::string &name, const glm::vec3 &value) const;
  void SetMat4(const std::string &name, const glm::mat4 &value) const;

private:
  void CheckCompileErrors(unsigned int shader, const std::string &type);
};
