#include "World.hpp"
#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

World::World(int seed, int renderDistance)
    : seed(seed), renderDistance(renderDistance), running(true) {
  workerThread = std::thread(&World::WorkerLoop, this);
}

World::~World() {
  running = false;
  workerThread.join();

  for (auto &[pos, chunk] : chunks)
    delete chunk;
  chunks.clear();

  std::lock_guard<std::mutex> dlock(deletionMutex);
  for (auto *chunk : deletionQueue)
    delete chunk;
  deletionQueue.clear();
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
  {
    std::lock_guard<std::mutex> lock(chunksMutex);
    if (chunks.count(chunkPos))
      return;
    chunks[chunkPos] = new Chunk(chunkPos);
  }
  std::lock_guard<std::mutex> lock(generateMutex);
  generateQueue.push(chunkPos);
}

void World::UnloadChunk(glm::ivec3 chunkPos) {
  std::lock_guard<std::mutex> lock(chunksMutex);
  auto it = chunks.find(chunkPos);
  if (it == chunks.end())
    return;
  it->second->markedForDeletion = true;
  {
    std::lock_guard<std::mutex> dlock(deletionMutex);
    deletionQueue.push_back(it->second);
  }
  chunks.erase(it);
}

void World::WorkerLoop() {
  while (running) {
    glm::ivec3 pos;
    bool hasWork = false;

    {
      std::lock_guard<std::mutex> lock(generateMutex);
      if (!generateQueue.empty()) {
        pos = generateQueue.front();
        generateQueue.pop();
        hasWork = true;
      }
    }

    if (!hasWork) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      continue;
    }

    Chunk *chunk = nullptr;
    {
      std::lock_guard<std::mutex> lock(chunksMutex);
      auto it = chunks.find(pos);
      if (it == chunks.end())
        continue;
      chunk = it->second;
    }

    if (!chunk || chunk->markedForDeletion)
      continue;

    if (chunk->state == ChunkState::Empty) {
      if (chunk->markedForDeletion)
        continue;
      chunk->Generate(seed);
    }

    if (chunk->state == ChunkState::Generated) {
      if (chunk->markedForDeletion)
        continue;
      ChunkMeshData data = chunk->BuildMeshData();
      {
        std::lock_guard<std::mutex> lock(uploadMutex);
        if (!chunk->markedForDeletion)
          uploadQueue.push({chunk, std::move(data)});
      }
    }
  }
}

void World::ProcessUploadQueue() {
  int uploads = 0;
  while (uploads < 4) {
    ReadyChunk ready;
    {
      std::lock_guard<std::mutex> lock(uploadMutex);
      if (uploadQueue.empty())
        break;
      ready = std::move(uploadQueue.front());
      uploadQueue.pop();
    }
    {
      std::lock_guard<std::mutex> lock(chunksMutex);
      if (chunks.count(ready.chunk->GetPosition()))
        ready.chunk->UploadMesh(ready.data);
    }
    uploads++;
  }
}

void World::ProcessDeletionQueue() {
  std::lock_guard<std::mutex> dlock(deletionMutex);
  deletionQueue.erase(std::remove_if(deletionQueue.begin(), deletionQueue.end(),
                                     [](Chunk *c) {
                                       ChunkState s = c->state.load();
                                       if (s == ChunkState::Generating ||
                                           s == ChunkState::Generated)
                                         return false;
                                       delete c;
                                       return true;
                                     }),
                      deletionQueue.end());
}

void World::Update(glm::vec3 playerPosition) {
  ProcessUploadQueue();
  ProcessDeletionQueue();

  glm::ivec3 playerChunk = WorldToChunkPos(playerPosition);

  for (int x = -renderDistance; x <= renderDistance; x++) {
    for (int z = -renderDistance; z <= renderDistance; z++) {
      for (int y = -2; y <= 2; y++) {
        LoadChunk(playerChunk + glm::ivec3(x, y, z));
      }
    }
  }

  std::vector<glm::ivec3> toUnload;
  {
    std::lock_guard<std::mutex> lock(chunksMutex);
    for (auto &[pos, chunk] : chunks) {
      glm::ivec3 diff = pos - playerChunk;
      if (abs(diff.x) > renderDistance || abs(diff.z) > renderDistance ||
          abs(diff.y) > 2) {
        toUnload.push_back(pos);
      }
    }
  }
  for (auto &pos : toUnload)
    UnloadChunk(pos);
}

void World::Draw(Shader &shader) {
  std::lock_guard<std::mutex> lock(chunksMutex);
  for (auto &[pos, chunk] : chunks)
    chunk->Draw();
}

void World::SetBlock(glm::vec3 worldPos, BlockType type) {
  glm::ivec3 chunkPos = WorldToChunkPos(worldPos);
  Chunk *chunk = nullptr;
  {
    std::lock_guard<std::mutex> lock(chunksMutex);
    auto it = chunks.find(chunkPos);
    if (it == chunks.end())
      return;
    chunk = it->second;
  }
  glm::ivec3 localPos = WorldToLocalPos(worldPos);
  chunk->SetBlock(localPos.x, localPos.y, localPos.z, type);

  std::lock_guard<std::mutex> lock(generateMutex);
  generateQueue.push(chunkPos);
}

BlockType World::GetBlock(glm::vec3 worldPos) const {
  glm::ivec3 chunkPos = WorldToChunkPos(worldPos);
  std::lock_guard<std::mutex> lock(const_cast<std::mutex &>(chunksMutex));
  auto it = chunks.find(chunkPos);
  if (it == chunks.end())
    return BlockType::Air;
  glm::ivec3 localPos = WorldToLocalPos(worldPos);
  return it->second->GetBlock(localPos.x, localPos.y, localPos.z);
}
