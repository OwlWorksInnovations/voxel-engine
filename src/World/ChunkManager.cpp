#include "ChunkManager.hpp"

ChunkManager::ChunkManager() {}

ChunkManager::~ChunkManager() {
    chunks.clear();
}

void ChunkManager::Update(glm::vec3 playerPos) {
    // For now, just a static set of chunks. 
    // Later we can add dynamic loading/unloading logic here.
}

void ChunkManager::Render(const glm::mat4& viewProj) {
    // Extract frustum planes from viewProj matrix
    glm::vec4 planes[6];
    // Left
    planes[0] = glm::vec4(viewProj[0][3] + viewProj[0][0], viewProj[1][3] + viewProj[1][0], viewProj[2][3] + viewProj[2][0], viewProj[3][3] + viewProj[3][0]);
    // Right
    planes[1] = glm::vec4(viewProj[0][3] - viewProj[0][0], viewProj[1][3] - viewProj[1][0], viewProj[2][3] - viewProj[2][0], viewProj[3][3] - viewProj[3][0]);
    // Bottom
    planes[2] = glm::vec4(viewProj[0][3] + viewProj[0][1], viewProj[1][3] + viewProj[1][1], viewProj[2][3] + viewProj[2][1], viewProj[3][3] + viewProj[3][1]);
    // Top
    planes[3] = glm::vec4(viewProj[0][3] - viewProj[0][1], viewProj[1][3] - viewProj[1][1], viewProj[2][3] - viewProj[2][1], viewProj[3][3] - viewProj[3][1]);
    // Near
    planes[4] = glm::vec4(viewProj[0][3] + viewProj[0][2], viewProj[1][3] + viewProj[1][2], viewProj[2][3] + viewProj[2][2], viewProj[3][3] + viewProj[3][2]);
    // Far
    planes[5] = glm::vec4(viewProj[0][3] - viewProj[0][2], viewProj[1][3] - viewProj[1][2], viewProj[2][3] - viewProj[2][2], viewProj[3][3] - viewProj[3][2]);

    // Normalize planes
    for (int i = 0; i < 6; i++) {
        float length = glm::length(glm::vec3(planes[i]));
        planes[i] /= length;
    }

    for (auto const& [pos, chunk] : chunks) {
        glm::vec3 min = glm::vec3(chunk->GetPosition());
        glm::vec3 max = min + glm::vec3(CHUNK_SIZE);

        bool inside = true;
        for (int i = 0; i < 6; i++) {
            glm::vec3 p;
            p.x = planes[i].x > 0 ? max.x : min.x;
            p.y = planes[i].y > 0 ? max.y : min.y;
            p.z = planes[i].z > 0 ? max.z : min.z;

            if (glm::dot(glm::vec3(planes[i]), p) + planes[i].w < 0) {
                inside = false;
                break;
            }
        }

        if (inside) {
            chunk->Render();
        }
    }
}

void ChunkManager::AddChunk(glm::ivec3 pos) {
    if (chunks.find(pos) == chunks.end()) {
        chunks[pos] = std::make_unique<Chunk>(pos * CHUNK_SIZE);
    }
}

void ChunkManager::RemoveChunk(glm::ivec3 pos) {
    chunks.erase(pos);
}

Chunk* ChunkManager::GetChunk(glm::ivec3 pos) {
    auto it = chunks.find(pos);
    if (it != chunks.end()) {
        return it->second.get();
    }
    return nullptr;
}

uint8_t ChunkManager::GetBlockAtWorldPos(glm::vec3 worldPos) {
    glm::ivec3 chunkPos = {
        (int)std::floor(worldPos.x / (float)CHUNK_SIZE),
        (int)std::floor(worldPos.y / (float)CHUNK_SIZE),
        (int)std::floor(worldPos.z / (float)CHUNK_SIZE)
    };

    Chunk* chunk = GetChunk(chunkPos);
    if (!chunk) return 0;

    glm::ivec3 localPos = {
        (int)std::floor(worldPos.x) % CHUNK_SIZE,
        (int)std::floor(worldPos.y) % CHUNK_SIZE,
        (int)std::floor(worldPos.z) % CHUNK_SIZE
    };

    // Handle negative modulo
    if (localPos.x < 0) localPos.x += CHUNK_SIZE;
    if (localPos.y < 0) localPos.y += CHUNK_SIZE;
    if (localPos.z < 0) localPos.z += CHUNK_SIZE;

    return chunk->GetBlock(localPos.x, localPos.y, localPos.z);
}
