#pragma once
#include <glad/glad.h>
#include <vector>

struct Mesh {
  unsigned int VAO, VBO, EBO;
  int indexCount;

  void create(const std::vector<float> &vertices,
              const std::vector<unsigned int> &indices) {
    indexCount = indices.size();
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    // UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
  }

  void destroy() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }
};

namespace DefaultMeshes {
static const std::vector<float> cubeVertices = {
    // positions          // uvs
    // front
    -0.5f,
    -0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    -0.5f,
    0.5f,
    1.0f,
    0.0f,
    0.5f,
    0.5f,
    0.5f,
    1.0f,
    1.0f,
    -0.5f,
    0.5f,
    0.5f,
    0.0f,
    1.0f,
    // back
    -0.5f,
    -0.5f,
    -0.5f,
    1.0f,
    0.0f,
    0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    -0.5f,
    0.0f,
    1.0f,
    -0.5f,
    0.5f,
    -0.5f,
    1.0f,
    1.0f,
    // left
    -0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    0.0f,
    -0.5f,
    -0.5f,
    0.5f,
    1.0f,
    0.0f,
    -0.5f,
    0.5f,
    0.5f,
    1.0f,
    1.0f,
    -0.5f,
    0.5f,
    -0.5f,
    0.0f,
    1.0f,
    // right
    0.5f,
    -0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    -0.5f,
    -0.5f,
    1.0f,
    0.0f,
    0.5f,
    0.5f,
    -0.5f,
    1.0f,
    1.0f,
    0.5f,
    0.5f,
    0.5f,
    0.0f,
    1.0f,
    // top
    -0.5f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    0.5f,
    1.0f,
    0.0f,
    0.5f,
    0.5f,
    -0.5f,
    1.0f,
    1.0f,
    -0.5f,
    0.5f,
    -0.5f,
    0.0f,
    1.0f,
    // bottom
    -0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    0.0f,
    0.5f,
    -0.5f,
    -0.5f,
    1.0f,
    0.0f,
    0.5f,
    -0.5f,
    0.5f,
    1.0f,
    1.0f,
    -0.5f,
    -0.5f,
    0.5f,
    0.0f,
    1.0f,
};

static const std::vector<unsigned int> cubeIndices = {
    0,  1,  2,  2,  3,  0,  // front
    4,  6,  5,  6,  4,  7,  // back
    8,  9,  10, 10, 11, 8,  // left
    12, 13, 14, 14, 15, 12, // right
    16, 17, 18, 18, 19, 16, // top
    20, 21, 22, 22, 23, 20, // bottom
};
} // namespace DefaultMeshes
