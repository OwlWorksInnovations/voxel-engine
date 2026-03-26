#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "../Render/Mesh.hpp"

const int CHUNK_SIZE = 16;

class Chunk {
public:
    Chunk(glm::ivec3 position);
    ~Chunk();

    void Render();
    void SetBlock(int x, int y, int z, uint8_t id);
    uint8_t GetBlock(int x, int y, int z) const;

    glm::ivec3 GetPosition() const { return worldPos; }

private:
    uint8_t blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
    glm::ivec3 worldPos; 
    Mesh mesh;

    void UpdateMesh();
};
