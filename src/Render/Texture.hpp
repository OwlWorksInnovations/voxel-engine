#pragma once

#include <glad/glad.h>
#include <string>

class Texture {
public:
  Texture();
  ~Texture();

  // Load a texture from a file path
  bool load(const std::string &path);

  // Bind the texture to a texture unit (default 0)
  void bind(unsigned int unit = 0) const;

  void unbind() const;

  inline unsigned int getID() const { return m_id; }
  inline int getWidth() const { return m_width; }
  inline int getHeight() const { return m_height; }
  inline bool isLoaded() const { return m_loaded; }

private:
  unsigned int m_id;
  int m_width;
  int m_height;
  int m_channels;
  bool m_loaded;
};
