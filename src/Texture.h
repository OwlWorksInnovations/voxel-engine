#ifndef TEXTURE_H
#define TEXTURE_H

#include "stb_image.h"
#include <glad/glad.h>
#include <iostream>

class Texture {
public:
  unsigned int textureID;

  Texture(const char *texturePath, GLenum format, int desiredChannels,
          GLenum wrapS, GLenum wrapT, GLenum minFilter, GLenum magFilter) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char *data =
        stbi_load(texturePath, &width, &height, &nrChannels, desiredChannels);
    if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    } else {
      std::cout << "Failed to load texture" << std::endl;
    };
    // Free up image
    stbi_image_free(data);
  };

private:
};
#endif
