#include "World.hpp"
#include "../Render/Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>

World::World(int seed, int renderDistance)
    : seed(seed), renderDistance(renderDistance) {}

World::~World() {
  for (auto &[pos, chunk] : chunks)
    delete chunk;
  chunks.clear();
}

glm::ivec3 World::WorldToChunkPos(glm::vec3 worldPos) const {
  return glm::ivec3((int)floor(worldPos.x / (CHUNK_SIZE * VOXEL_SIZE)),
                    (int)floor(worldPos.y / (CHUNK_SIZE * VOXEL_SIZE)),
                    (int)floor(worldPos.z / (CHUNK_SIZE * VOXEL_SIZE)));
}

glm::ivec3 World::WorldToLocalPos(glm::vec3 worldPos) const {
  glm::ivec3 chunkPos = WorldToChunkPos(worldPos);
  return glm::ivec3(
      (int)floor(worldPos.x / VOXEL_SIZE) - chunkPos.x * CHUNK_SIZE,
      (int)floor(worldPos.y / VOXEL_SIZE) - chunkPos.y * CHUNK_SIZE,
      (int)floor(worldPos.z / VOXEL_SIZE) - chunkPos.z * CHUNK_SIZE);
}

void World::LoadChunk(glm::ivec3 chunkPos) {
  if (chunks.count(chunkPos))
    return;
  Chunk *chunk = new Chunk(chunkPos);
  chunk->Generate(seed);
  chunks[chunkPos] = chunk;
}

void World::UnloadChunk(glm::ivec3 chunkPos) {
  auto it = chunks.find(chunkPos);
  if (it == chunks.end())
    return;
  delete it->second;
  chunks.erase(it);
}

void World::Update(glm::vec3 playerPosition) {
  glm::ivec3 playerChunk = WorldToChunkPos(playerPosition);

  // load chunks in render distance
  for (int x = -renderDistance; x <= renderDistance; x++) {
    for (int z = -renderDistance; z <= renderDistance; z++) {
      for (int y = -2; y <= 2; y++) {
        glm::ivec3 chunkPos = playerChunk + glm::ivec3(x, y, z);
        LoadChunk(chunkPos);
      }
    }
  }

  // unload chunks outside render distance
  std::vector<glm::ivec3> toUnload;
  for (auto &[pos, chunk] : chunks) {
    glm::ivec3 diff = pos - playerChunk;
    if (abs(diff.x) > renderDistance || abs(diff.z) > renderDistance ||
        abs(diff.y) > 2) {
      toUnload.push_back(pos);
    }
  }
  for (auto &pos : toUnload)
    UnloadChunk(pos);
}

void World::Draw(Shader &shader) {
  for (auto &[pos, chunk] : chunks) {
    chunk->Draw();
  }
}

void World::SetBlock(glm::vec3 worldPos, BlockType type) {
  glm::ivec3 chunkPos = WorldToChunkPos(worldPos);
  auto it = chunks.find(chunkPos);
  if (it == chunks.end())
    return;
  glm::ivec3 localPos = WorldToLocalPos(worldPos);
  it->second->SetBlock(localPos.x, localPos.y, localPos.z, type);
}

BlockType World::GetBlock(glm::vec3 worldPos) const {
  glm::ivec3 chunkPos = WorldToChunkPos(worldPos);
  auto it = chunks.find(chunkPos);
  if (it == chunks.end())
    return BlockType::Air;
  glm::ivec3 localPos = WorldToLocalPos(worldPos);
  return it->second->GetBlock(localPos.x, localPos.y, localPos.z);
}
