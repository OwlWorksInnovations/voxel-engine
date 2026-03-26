#include "Chunk.hpp"
#include <FastNoiseLite.h>
#include <iostream>

Chunk::Chunk(glm::ivec3 position) : worldPos(position) {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.05f);

    // Initialize blocks based on noise
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            float nx = (float)(worldPos.x + x);
            float nz = (float)(worldPos.z + z);
            float noiseValue = noise.GetNoise(nx, nz);
            int height = (int)((noiseValue + 1.0f) * 0.5f * CHUNK_SIZE);
            
            for (int y = 0; y < CHUNK_SIZE; y++) {
                if (y < height) {
                    blocks[x][y][z] = 1; // Solid block
                } else {
                    blocks[x][y][z] = 0; // Air
                }
            }
        }
    }
    UpdateMesh();
}

Chunk::~Chunk() {
    mesh.Clear();
}

void Chunk::Render() {
    mesh.Draw();
}

void Chunk::SetBlock(int x, int y, int z, uint8_t id) {
    if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE) {
        blocks[x][y][z] = id;
        UpdateMesh();
    }
}

uint8_t Chunk::GetBlock(int x, int y, int z) const {
    if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE) {
        return blocks[x][y][z];
    }
    return 0;
}

void Chunk::UpdateMesh() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int vertexOffset = 0;

    auto getBlockSafe = [&](int x, int y, int z) -> uint8_t {
        if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) return 0;
        return blocks[x][y][z];
    };

    // Greedy meshing implementation
    for (int d = 0; d < 3; d++) { // Iterate over dimensions X, Y, Z
        int u = (d + 1) % 3;
        int v = (d + 2) % 3;
        int x[3] = {0};
        int q[3] = {0};
        int mask[CHUNK_SIZE * CHUNK_SIZE];
        q[d] = 1;

        for (x[d] = -1; x[d] < CHUNK_SIZE; ) {
            int n = 0;
            for (x[v] = 0; x[v] < CHUNK_SIZE; x[v]++) {
                for (x[u] = 0; x[u] < CHUNK_SIZE; x[u]++) {
                    uint8_t a = getBlockSafe(x[0], x[1], x[2]);
                    uint8_t b = getBlockSafe(x[0] + q[0], x[1] + q[1], x[2] + q[2]);
                    
                    if ((a != 0) == (b != 0)) {
                        mask[n++] = 0;
                    } else if (a != 0) {
                        mask[n++] = a; // Face pointing in positive direction
                    } else {
                        mask[n++] = -b; // Face pointing in negative direction
                    }
                }
            }

            x[d]++;
            n = 0;
            for (int j = 0; j < CHUNK_SIZE; j++) {
                for (int i = 0; i < CHUNK_SIZE; ) {
                    if (mask[n]) {
                        int w, h;
                        int type = mask[n];

                        // Compute width
                        for (w = 1; i + w < CHUNK_SIZE && mask[n + w] == type; w++);

                        // Compute height
                        bool done = false;
                        for (h = 1; j + h < CHUNK_SIZE; h++) {
                            for (int k = 0; k < w; k++) {
                                if (mask[n + k + h * CHUNK_SIZE] != type) {
                                    done = true;
                                    break;
                                }
                            }
                            if (done) break;
                        }

                        // Add quad
                        x[u] = i; x[v] = j;
                        int du[3] = {0}; du[u] = w;
                        int dv[3] = {0}; dv[v] = h;

                        glm::vec3 p0 = glm::vec3(worldPos.x + x[0], worldPos.y + x[1], worldPos.z + x[2]);

                        glm::vec3 p1 = p0 + glm::vec3(du[0], du[1], du[2]);
                        glm::vec3 p2 = p0 + glm::vec3(du[0] + dv[0], du[1] + dv[1], du[2] + dv[2]);
                        glm::vec3 p3 = p0 + glm::vec3(dv[0], dv[1], dv[2]);

                        // UVs based on width and height
                        float uw = (float)w;
                        float vh = (float)h;

                        if (type > 0) {
                            vertices.push_back({p0.x, p0.y, p0.z, 0.0f, 0.0f});
                            vertices.push_back({p1.x, p1.y, p1.z, uw, 0.0f});
                            vertices.push_back({p2.x, p2.y, p2.z, uw, vh});
                            vertices.push_back({p3.x, p3.y, p3.z, 0.0f, vh});
                        } else {
                            // Reverse order for other side
                            vertices.push_back({p0.x, p0.y, p0.z, 0.0f, 0.0f});
                            vertices.push_back({p3.x, p3.y, p3.z, 0.0f, vh});
                            vertices.push_back({p2.x, p2.y, p2.z, uw, vh});
                            vertices.push_back({p1.x, p1.y, p1.z, uw, 0.0f});
                        }

                        indices.push_back(vertexOffset + 0);
                        indices.push_back(vertexOffset + 1);
                        indices.push_back(vertexOffset + 2);
                        indices.push_back(vertexOffset + 0);
                        indices.push_back(vertexOffset + 2);
                        indices.push_back(vertexOffset + 3);
                        vertexOffset += 4;

                        // Clear mask
                        for (int l = 0; l < h; l++) {
                            for (int k = 0; k < w; k++) {
                                mask[n + k + l * CHUNK_SIZE] = 0;
                            }
                        }
                        
                        i += w;
                        n += w;
                    } else {
                        i++;
                        n++;
                    }
                }
            }
        }
    }

    mesh.SetData(vertices, indices);
}
