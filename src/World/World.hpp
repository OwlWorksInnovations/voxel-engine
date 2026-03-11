#pragma once
#include "../Render/Shader.hpp"
#include "Chunk.hpp"
#include <atomic>
#include <glm/glm.hpp>
#include <mutex>
#include <queue>
#include <thread>
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

private:
  std::unordered_map<glm::ivec3, Chunk *, IVec3Hash> chunks;
  int seed;
  int renderDistance;

  // threading
  std::thread workerThread;
  std::atomic<bool> running;
  std::mutex chunksMutex;

  // generate queue
  std::queue<glm::ivec3> generateQueue;
  std::mutex generateMutex;

  // upload queue
  struct ReadyChunk {
    Chunk *chunk;
    ChunkMeshData data;
  };
  std::queue<ReadyChunk> uploadQueue;
  std::mutex uploadMutex;

  // deletion queue
  std::vector<Chunk *> deletionQueue;
  std::mutex deletionMutex;

  void WorkerLoop();
  void ProcessUploadQueue();
  void ProcessDeletionQueue();

  glm::ivec3 WorldToChunkPos(glm::vec3 worldPos) const;
  glm::ivec3 WorldToLocalPos(glm::vec3 worldPos) const;
  void LoadChunk(glm::ivec3 chunkPos);
  void UnloadChunk(glm::ivec3 chunkPos);
};
