#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>

struct Chunk {
  int chunkX, chunkZ;
  int indexCount;
  int voxels[64][64][64];

  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;

  void destroy() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }
};

// Helper for unordered_map with pair as key
struct ChunkPosHash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
    }
};

class ChunkManager {
public:
  std::unordered_map<std::pair<int, int>, Chunk*, ChunkPosHash> chunkMap;
  std::vector<Chunk*> chunks;

  ~ChunkManager() {
    for (auto chunk : chunks) {
        chunk->destroy();
        delete chunk;
    }
  }

  void createChunk(int cx, int cz, class PerlinNoise& perlin, float maxHeight);

  void buildMesh(Chunk &chunk, std::vector<float> &vertices,
                 std::vector<unsigned int> &indices) {
    for (int x = 0; x < 64; x++) {
      for (int y = 0; y < 64; y++) {
        for (int z = 0; z < 64; z++) {

          if (chunk.voxels[x][y][z] == 0)
            continue;

          float worldX = (float)x + chunk.chunkX * 64;
          float worldY = (float)y;
          float worldZ = (float)z + chunk.chunkZ * 64;

          // Top
          if (y + 1 >= 64 || chunk.voxels[x][y + 1][z] == 0) {
            unsigned int base = vertices.size() / 8;
            vertices.insert(vertices.end(), {
                worldX,     worldY + 1.0f, worldZ,        0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                worldX + 1, worldY + 1.0f, worldZ,        1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                worldX + 1, worldY + 1.0f, worldZ + 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                worldX,     worldY + 1.0f, worldZ + 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            });
            indices.insert(indices.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
          }

          // Bottom
          if (y - 1 < 0 || chunk.voxels[x][y - 1][z] == 0) {
            unsigned int base = vertices.size() / 8;
            vertices.insert(vertices.end(), {
                worldX,     worldY, worldZ,        0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
                worldX + 1, worldY, worldZ,        1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
                worldX + 1, worldY, worldZ + 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
                worldX,     worldY, worldZ + 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
            });
            indices.insert(indices.end(), {base, base + 2, base + 1, base, base + 3, base + 2});
          }

          // Right
          if (x + 1 >= 64 || chunk.voxels[x + 1][y][z] == 0) {
            unsigned int base = vertices.size() / 8;
            vertices.insert(vertices.end(), {
                worldX + 1, worldY,     worldZ,        0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                worldX + 1, worldY + 1, worldZ,        1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                worldX + 1, worldY + 1, worldZ + 1,    1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                worldX + 1, worldY,     worldZ + 1,    0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            });
            indices.insert(indices.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
          }

          // Left
          if (x - 1 < 0 || chunk.voxels[x - 1][y][z] == 0) {
            unsigned int base = vertices.size() / 8;
            vertices.insert(vertices.end(), {
                worldX, worldY,     worldZ,        0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
                worldX, worldY + 1, worldZ,        1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
                worldX, worldY + 1, worldZ + 1,    1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
                worldX, worldY,     worldZ + 1,    0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            });
            indices.insert(indices.end(), {base, base + 2, base + 1, base, base + 3, base + 2});
          }

          // Front
          if (z + 1 >= 64 || chunk.voxels[x][y][z + 1] == 0) {
            unsigned int base = vertices.size() / 8;
            vertices.insert(vertices.end(), {
                worldX,     worldY,     worldZ + 1,    0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                worldX + 1, worldY,     worldZ + 1,    1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                worldX + 1, worldY + 1, worldZ + 1,    1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                worldX,     worldY + 1, worldZ + 1,    0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            });
            indices.insert(indices.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
          }

          // Back
          if (z - 1 < 0 || chunk.voxels[x][y][z - 1] == 0) {
            unsigned int base = vertices.size() / 8;
            vertices.insert(vertices.end(), {
                worldX,     worldY,     worldZ, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
                worldX + 1, worldY,     worldZ, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
                worldX + 1, worldY + 1, worldZ, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
                worldX,     worldY + 1, worldZ, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
            });
            indices.insert(indices.end(), {base, base + 2, base + 1, base, base + 3, base + 2});
          }
        }
      }
    }
  }
};
