#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

struct Chunk {
  int ID;
  int indexCount;
  int voxels[16][16][16];
  glm::vec3 position;

  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;
};

class ChunkManager {
public:
  int nextID = 0;
  Chunk chunk;
  std::vector<Chunk> chunks;

  void createChunk() {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    buildMesh(chunk, vertices, indices);

    // Generate buffer and array objects
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind buffers and upload vertex/index data to the GPU
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    // Tell the VAO how vertex data is laid out in memory
    // Attribute 0: position (3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    // Attribute 2: texture coordinates (2 floats, offset by 3 floats)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    chunk.ID = nextID++;
    chunk.indexCount = indices.size();
    chunk.VBO = VBO;
    chunk.VAO = VAO;
    chunk.EBO = EBO;

    // Unbind VBO and VAO (EBO stays bound to VAO)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    chunks.push_back(chunk);
  }

  void buildMesh(Chunk &chunk, std::vector<float> &vertices,
                 std::vector<unsigned int> &indices) {
    // There must be a better way I just need to find it because this is sooooo
    // cubersome
    for (int x = 0; x < 16; x++) {
      for (int y = 0; y < 16; y++) {
        for (int z = 0; z < 16; z++) {

          if (chunk.voxels[x][y][z] == 0)
            continue;

          // Top
          if (y + 1 >= 16 || chunk.voxels[x][y + 1][z] == 0) {
            unsigned int base = vertices.size() / 5;
            vertices.insert(
                vertices.end(),
                {
                    (float)x,     (float)(y + 1), (float)z,       0.0f, 0.0f,
                    (float)x + 1, (float)(y + 1), (float)z,       1.0f, 0.0f,
                    (float)x + 1, (float)(y + 1), (float)(z + 1), 1.0f, 1.0f,
                    (float)x,     (float)(y + 1), (float)(z + 1), 0.0f, 1.0f,
                });
            indices.insert(indices.end(), {base, base + 1, base + 2, base,
                                           base + 2, base + 3});
          }

          // Bottom
          if (y - 1 < 0 || chunk.voxels[x][y - 1][z] == 0) {
            unsigned int base = vertices.size() / 5;
            vertices.insert(
                vertices.end(),
                {
                    (float)x,     (float)y, (float)z,       0.0f, 0.0f,
                    (float)x + 1, (float)y, (float)z,       1.0f, 0.0f,
                    (float)x + 1, (float)y, (float)(z + 1), 1.0f, 1.0f,
                    (float)x,     (float)y, (float)(z + 1), 0.0f, 1.0f,
                });
            indices.insert(indices.end(), {base, base + 2, base + 1, base,
                                           base + 3, base + 2});
          }

          // Right
          if (x + 1 >= 16 || chunk.voxels[x + 1][y][z] == 0) {
            unsigned int base = vertices.size() / 5;
            vertices.insert(
                vertices.end(),
                {
                    (float)(x + 1), (float)y,     (float)z,       0.0f, 0.0f,
                    (float)(x + 1), (float)y + 1, (float)z,       1.0f, 0.0f,
                    (float)(x + 1), (float)y + 1, (float)(z + 1), 1.0f, 1.0f,
                    (float)(x + 1), (float)y,     (float)(z + 1), 0.0f, 1.0f,
                });
            indices.insert(indices.end(), {base, base + 1, base + 2, base,
                                           base + 2, base + 3});
          }

          // Left
          if (x - 1 < 0 || chunk.voxels[x - 1][y][z] == 0) {
            unsigned int base = vertices.size() / 5;
            vertices.insert(
                vertices.end(),
                {
                    (float)x, (float)y,     (float)z,       0.0f, 0.0f,
                    (float)x, (float)y + 1, (float)z,       1.0f, 0.0f,
                    (float)x, (float)y + 1, (float)(z + 1), 1.0f, 1.0f,
                    (float)x, (float)y,     (float)(z + 1), 0.0f, 1.0f,
                });
            indices.insert(indices.end(), {base, base + 2, base + 1, base,
                                           base + 3, base + 2});
          }

          // Front
          if (z + 1 >= 16 || chunk.voxels[x][y][z + 1] == 0) {
            unsigned int base = vertices.size() / 5;
            vertices.insert(
                vertices.end(),
                {
                    (float)x,     (float)y,     (float)(z + 1), 0.0f, 0.0f,
                    (float)x + 1, (float)y,     (float)(z + 1), 1.0f, 0.0f,
                    (float)x + 1, (float)y + 1, (float)(z + 1), 1.0f, 1.0f,
                    (float)x,     (float)y + 1, (float)(z + 1), 0.0f, 1.0f,
                });
            indices.insert(indices.end(), {base, base + 1, base + 2, base,
                                           base + 2, base + 3});
          }

          // Back
          if (z - 1 < 0 || chunk.voxels[x][y][z - 1] == 0) {
            unsigned int base = vertices.size() / 5;
            vertices.insert(
                vertices.end(),
                {
                    (float)x,     (float)y,     (float)z, 0.0f, 0.0f,
                    (float)x + 1, (float)y,     (float)z, 1.0f, 0.0f,
                    (float)x + 1, (float)y + 1, (float)z, 1.0f, 1.0f,
                    (float)x,     (float)y + 1, (float)z, 0.0f, 1.0f,
                });
            indices.insert(indices.end(), {base, base + 2, base + 1, base,
                                           base + 3, base + 2});
          }
        }
      }
    }
  }
};
