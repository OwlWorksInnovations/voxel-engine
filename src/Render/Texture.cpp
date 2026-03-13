#include "Texture.hpp"
#include <stb_image.h>

#include <iostream>

Texture::Texture()
    : m_id(0), m_width(0), m_height(0), m_channels(0), m_loaded(false) {}

Texture::~Texture() {
  if (m_loaded)
    glDeleteTextures(1, &m_id);
}

bool Texture::load(const std::string &path) {
  // Flip vertically so textures aren't upside down
  stbi_set_flip_vertically_on_load(true);

  unsigned char *data =
      stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);

  if (!data) {
    std::cerr << "[Texture] Failed to load: " << path << "\n";
    std::cerr << "[Texture] stb_image error: " << stbi_failure_reason() << "\n";
    return false;
  }

  // Pick the right format based on number of channels
  GLenum format = GL_RGB;
  if (m_channels == 1)
    format = GL_RED;
  else if (m_channels == 3)
    format = GL_RGB;
  else if (m_channels == 4)
    format = GL_RGBA;

  glGenTextures(1, &m_id);
  glBindTexture(GL_TEXTURE_2D, m_id);

  // Wrapping
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Filtering — nearest for pixel art / voxel look, linear for smooth
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(data);

  m_loaded = true;
  return true;
}

void Texture::bind(unsigned int unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
