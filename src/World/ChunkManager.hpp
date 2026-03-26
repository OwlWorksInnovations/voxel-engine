#pragma once
#include <map>
#include <memory>
#include <glm/glm.hpp>
#include "Chunk.hpp"

struct ChunkPosComparator {
    bool operator()(const glm::ivec3& a, const glm::ivec3& b) const {
        if (a.x != b.x) return a.x < b.x;
        if (a.y != b.y) return a.y < b.y;
        return a.z < b.z;
    }
};

class ChunkManager {
public:
    ChunkManager();
    ~ChunkManager();

    void Update(glm::vec3 playerPos);
    void Render(const glm::mat4& viewProj);

    void AddChunk(glm::ivec3 pos);
    void RemoveChunk(glm::ivec3 pos);
    Chunk* GetChunk(glm::ivec3 pos);

    uint8_t GetBlockAtWorldPos(glm::vec3 worldPos);
    void SetBlockAtWorldPos(glm::vec3 worldPos, uint8_t id);

    bool Raycast(glm::vec3 origin, glm::vec3 direction, float maxDist, glm::ivec3& outBlockPos, glm::ivec3& outNormal);

private:
    std::map<glm::ivec3, std::unique_ptr<Chunk>, ChunkPosComparator> chunks;
};
