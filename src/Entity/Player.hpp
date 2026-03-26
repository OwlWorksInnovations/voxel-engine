#pragma once
#include <glm/glm.hpp>
#include "../Render/Camera.hpp"
#include "../Physics/AABB.hpp"
#include "../World/ChunkManager.hpp"

class Player {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    
    Player(glm::vec3 startPos);

    void Update(float deltaTime, ChunkManager& chunkManager);
    void ProcessKeyboard(float deltaTime);
    void ProcessMouse(float xOffset, float yOffset);

    Camera& GetCamera() { return camera; }
    const AABB& GetAABB() const { return playerAABB; }

private:
    Camera camera;
    AABB playerAABB;
    bool isGrounded = false;
    
    // Player physical dimensions
    const glm::vec3 playerSize = glm::vec3(0.6f, 1.8f, 0.6f);
    const float walkSpeed = 100.0f;
    const float gravity = 20.0f;
    const float jumpForce = 8.0f;

    void ResolveCollisions(glm::vec3 oldPos, ChunkManager& chunkManager);
    bool CheckCollision(glm::vec3 pos, ChunkManager& chunkManager);
};
