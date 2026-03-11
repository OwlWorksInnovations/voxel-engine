#include "Chunk.hpp"
#include "FastNoiseLite.h"
#include <glm/glm.hpp>

Chunk::Chunk(glm::ivec3 position) : position(position), dirty(true) {
  blocks.fill(BlockType::Air);
}

void Chunk::Generate(int seed) {
  FastNoiseLite noise;
  noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  noise.SetFrequency(0.03f);
  noise.SetSeed(seed);

  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int z = 0; z < CHUNK_SIZE; z++) {
      // world position of this column
      float wx = (position.x * CHUNK_SIZE + x) * VOXEL_SIZE;
      float wz = (position.z * CHUNK_SIZE + z) * VOXEL_SIZE;

      // height in voxels (0 to CHUNK_SIZE)
      float n = noise.GetNoise(wx, wz);                   // -1 to 1
      int height = (int)((n + 1.0f) * 0.5f * CHUNK_SIZE); // 0 to CHUNK_SIZE

      for (int y = 0; y < CHUNK_SIZE; y++) {
        int wy = position.y * CHUNK_SIZE + y;

        if (wy > height)
          SetBlock(x, y, z, BlockType::Air);
        else if (wy == height)
          SetBlock(x, y, z, BlockType::Grass);
        else if (wy >= height - 3)
          SetBlock(x, y, z, BlockType::Dirt);
        else
          SetBlock(x, y, z, BlockType::Stone);
      }
    }
  }
  dirty = true;
}

bool Chunk::InBounds(int x, int y, int z) const {
  return x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE && z >= 0 &&
         z < CHUNK_SIZE;
}

void Chunk::SetBlock(int x, int y, int z, BlockType type) {
  if (!InBounds(x, y, z))
    return;
  blocks[Index(x, y, z)] = type;
  dirty = true;
}

BlockType Chunk::GetBlock(int x, int y, int z) const {
  if (!InBounds(x, y, z))
    return BlockType::Air;
  return blocks[Index(x, y, z)];
}

bool Chunk::IsFaceVisible(int x, int y, int z) const {
  if (!InBounds(x, y, z))
    return true; // chunk border, always draw
  return IsAir(GetBlock(x, y, z));
}

void Chunk::AddFace(std::vector<Vertex> &vertices,
                    std::vector<unsigned int> &indices, glm::vec3 pos,
                    glm::vec3 normal) {

  unsigned int base = vertices.size();

  // build a quad perpendicular to normal
  glm::vec3 up, right;

  if (glm::abs(normal.y) > 0.5f) {
    right = glm::vec3(1, 0, 0);
    up = glm::vec3(0, 0, 1);
  } else if (glm::abs(normal.x) > 0.5f) {
    right = glm::vec3(0, 1, 0);
    up = glm::vec3(0, 0, 1);
  } else {
    right = glm::vec3(1, 0, 0);
    up = glm::vec3(0, 1, 0);
  }

  glm::vec3 center = pos + normal * (VOXEL_SIZE * 0.5f);
  float h = VOXEL_SIZE * 0.5f;

  glm::vec3 p0 = center - right * h - up * h;
  glm::vec3 p1 = center + right * h - up * h;
  glm::vec3 p2 = center + right * h + up * h;
  glm::vec3 p3 = center - right * h + up * h;

  vertices.push_back({p0.x, p0.y, p0.z, 0.0f, 0.0f});
  vertices.push_back({p1.x, p1.y, p1.z, 1.0f, 0.0f});
  vertices.push_back({p2.x, p2.y, p2.z, 1.0f, 1.0f});
  vertices.push_back({p3.x, p3.y, p3.z, 0.0f, 1.0f});

  indices.push_back(base + 0);
  indices.push_back(base + 1);
  indices.push_back(base + 2);
  indices.push_back(base + 0);
  indices.push_back(base + 2);
  indices.push_back(base + 3);
}

void Chunk::BuildMesh() {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  static const glm::ivec3 directions[6] = {{1, 0, 0},  {-1, 0, 0}, {0, 1, 0},
                                           {0, -1, 0}, {0, 0, 1},  {0, 0, -1}};

  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
      for (int z = 0; z < CHUNK_SIZE; z++) {
        if (IsAir(GetBlock(x, y, z)))
          continue;

        glm::vec3 worldPos =
            glm::vec3((position.x * CHUNK_SIZE + x) * VOXEL_SIZE,
                      (position.y * CHUNK_SIZE + y) * VOXEL_SIZE,
                      (position.z * CHUNK_SIZE + z) * VOXEL_SIZE);

        for (auto &dir : directions) {
          if (IsFaceVisible(x + dir.x, y + dir.y, z + dir.z)) {
            AddFace(vertices, indices, worldPos, glm::vec3(dir));
          }
        }
      }
    }
  }

  mesh.SetData(vertices, indices);
  dirty = false;
}

void Chunk::Draw() {
  if (dirty)
    BuildMesh();
  mesh.Draw();
}
