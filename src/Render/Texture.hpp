#pragma once
#include <glad/glad.h>
#include <string>

class Texture {
public:
  unsigned int ID;

  Texture(const std::string &path);
  ~Texture();

  void Bind(unsigned int slot = 0);
  void Unbind();
};
