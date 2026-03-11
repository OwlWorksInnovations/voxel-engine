#pragma once
#include "../Render/Mesh.hpp"
#include "Block.hpp"
#include <array>
#include <glm/glm.hpp>

constexpr int CHUNK_SIZE = 32;
constexpr float VOXEL_SIZE = 0.25f;

class Chunk {
public:
  Chunk(glm::ivec3 position);

  void Generate(int seed);
  void BuildMesh();
  void Draw();

  void SetBlock(int x, int y, int z, BlockType type);
  BlockType GetBlock(int x, int y, int z) const;

  glm::ivec3 GetPosition() const { return position; }
  bool IsDirty() const { return dirty; }

private:
  std::array<BlockType, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> blocks;
  Mesh mesh;
  glm::ivec3 position; // chunk position in chunk coordinates
  bool dirty;          // needs mesh rebuild

  int Index(int x, int y, int z) const {
    return x + CHUNK_SIZE * (y + CHUNK_SIZE * z);
  }
  bool InBounds(int x, int y, int z) const;
  bool IsFaceVisible(int x, int y, int z) const;
  void AddFace(std::vector<Vertex> &vertices,
               std::vector<unsigned int> &indices, glm::vec3 pos,
               glm::vec3 normal);
};
