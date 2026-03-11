#include "Texture.hpp"
#include "stb_image.h"
#include <iostream>

Texture::Texture(const std::string &path) {
  glGenTextures(1, &ID);
  glBindTexture(GL_TEXTURE_2D, ID);

  // wrapping
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // load image
  int width, height, channels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);

  if (data) {
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cerr << "ERROR::TEXTURE::Failed to load: " << path << std::endl;
  }

  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() { glDeleteTextures(1, &ID); }

void Texture::Bind(unsigned int slot) {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
