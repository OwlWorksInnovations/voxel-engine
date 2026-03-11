#pragma once
#include "../Render/Shader.hpp"
#include "Chunk.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

struct IVec3Hash {
  size_t operator()(const glm::ivec3 &v) const {
    size_t h1 = std::hash<int>()(v.x);
    size_t h2 = std::hash<int>()(v.y);
    size_t h3 = std::hash<int>()(v.z);
    return h1 ^ (h2 << 16) ^ (h3 << 32);
  }
};

class World {
public:
  World(int seed = 1337, int renderDistance = 4);
  ~World();

  void Update(glm::vec3 playerPosition);
  void Draw(Shader &shader);

  void SetBlock(glm::vec3 worldPos, BlockType type);
  BlockType GetBlock(glm::vec3 worldPos) const;

  void SetBlockVoxel(glm::ivec3 voxelPos, BlockType type);
  BlockType GetBlockVoxel(glm::ivec3 voxelPos) const;

private:
  std::unordered_map<glm::ivec3, Chunk *, IVec3Hash> chunks;
  int seed;
  int renderDistance;

  void GenerateChunk(glm::ivec3 chunkPos);
  void BuildChunkMesh(Chunk *chunk);
  void UnloadChunk(glm::ivec3 chunkPos);

  glm::ivec3 WorldToChunkPos(glm::vec3 worldPos) const;
  glm::ivec3 WorldToLocalPos(glm::vec3 worldPos) const;
};
