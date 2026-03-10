#pragma once
#include "Chunk.h"
#include <cmath>
#include <glm/glm.hpp>
#include <vector>

class PerlinNoise {
public:
  float scale = 32.0f;

  float noise2d(float x, float z) {
    // Simple deterministic hash-based noise
    return fract(sin(dot(glm::vec2(x, z), glm::vec2(12.9898, 78.233))) * 43758.5453);
  }

  // Improved value noise for smoother terrain
  float smoothNoise(float x, float z) {
    float x_int = floor(x);
    float z_int = floor(z);
    float x_fract = x - x_int;
    float z_fract = z - z_int;

    float v1 = hash(x_int, z_int);
    float v2 = hash(x_int + 1, z_int);
    float v3 = hash(x_int, z_int + 1);
    float v4 = hash(x_int + 1, z_int + 1);

    float i1 = lerp(v1, v2, fade(x_fract));
    float i2 = lerp(v3, v4, fade(x_fract));

    return lerp(i1, i2, fade(z_fract));
  }

  float getNoise(float x, float z) {
    float total = 0;
    float frequency = 1.0f / scale;
    float amplitude = 1.0f;
    for(int i = 0; i < 4; i++) {
        total += smoothNoise(x * frequency, z * frequency) * amplitude;
        frequency *= 2.0f;
        amplitude *= 0.5f;
    }
    return total;
  }

  void generateVoxelData(Chunk &chunk, float maxHeight) {
    for (int x = 0; x < 64; x++) {
      for (int z = 0; z < 64; z++) {
        float worldX = (float)(x + chunk.chunkX * 64);
        float worldZ = (float)(z + chunk.chunkZ * 64);
        
        float n = getNoise(worldX, worldZ);
        int height = (int)(n * maxHeight);

        for (int y = 0; y < 64; y++) {
          chunk.voxels[x][y][z] = (y <= height) ? 1 : 0;
        }
      }
    }
  }

private:
  float hash(float x, float z) {
    float h = dot(glm::vec2(x, z), glm::vec2(127.1, 311.7));
    return fract(sin(h) * 43758.5453123);
  }

  float fract(float x) { return x - floor(x); }
  float lerp(float a, float b, float t) { return a + t * (b - a); }
  float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
  float dot(glm::vec2 a, glm::vec2 b) { return a.x * b.x + a.y * b.y; }
};

// Implement createChunk here to avoid circular dependency issues
inline void ChunkManager::createChunk(int cx, int cz, PerlinNoise& perlin, float maxHeight) {
    if (chunkMap.find({cx, cz}) != chunkMap.end()) return;

    Chunk* newChunk = new Chunk();
    newChunk->chunkX = cx;
    newChunk->chunkZ = cz;
    
    perlin.generateVoxelData(*newChunk, maxHeight);

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    buildMesh(*newChunk, vertices, indices);

    glGenVertexArrays(1, &newChunk->VAO);
    glGenBuffers(1, &newChunk->VBO);
    glGenBuffers(1, &newChunk->EBO);

    glBindVertexArray(newChunk->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, newChunk->VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newChunk->EBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(3);

    newChunk->indexCount = indices.size();
    glBindVertexArray(0);

    chunkMap[{cx, cz}] = newChunk;
    chunks.push_back(newChunk);
}
