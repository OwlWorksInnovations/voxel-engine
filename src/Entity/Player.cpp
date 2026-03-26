#include "Player.hpp"
#include "../Core/Input.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>

Player::Player(glm::vec3 startPos) 
    : position(startPos), velocity(0.0f), camera(startPos), playerAABB(startPos - playerSize * 0.5f, startPos + playerSize * 0.5f) {
}

void Player::Update(float deltaTime, ChunkManager& chunkManager) {
    // Apply gravity
    if (!isGrounded) {
        velocity.y -= gravity * deltaTime;
    }

    glm::vec3 oldPos = position;

    // Move step by step for each axis to handle sliding
    // X axis
    position.x += velocity.x * deltaTime;
    if (CheckCollision(position, chunkManager)) {
        position.x = oldPos.x;
        velocity.x = 0;
    }

    // Y axis
    position.y += velocity.y * deltaTime;
    isGrounded = false;
    if (CheckCollision(position, chunkManager)) {
        if (velocity.y < 0) isGrounded = true;
        position.y = oldPos.y;
        velocity.y = 0;
    }

    // Z axis
    position.z += velocity.z * deltaTime;
    if (CheckCollision(position, chunkManager)) {
        position.z = oldPos.z;
        velocity.z = 0;
    }

    // Apply friction/drag to horizontal velocity
    velocity.x *= 0.9f;
    velocity.z *= 0.9f;

    // Update camera position to follow player (head position)
    camera.position = position + glm::vec3(0.0f, 0.75f, 0.0f);
    
    // Update AABB
    playerAABB.min = position - glm::vec3(playerSize.x * 0.5f, 0.9f, playerSize.z * 0.5f);
    playerAABB.max = position + glm::vec3(playerSize.x * 0.5f, 0.9f, playerSize.z * 0.5f);
}

void Player::ProcessKeyboard(float deltaTime) {
    glm::vec3 front = camera.GetFront();
    front = glm::normalize(glm::vec3(front.x, 0, front.z));
    glm::vec3 right = camera.GetRight();

    float accel = walkSpeed * 20.0f; // Increased acceleration

    if (Input::IsKeyHeld(GLFW_KEY_W)) velocity += front * accel * deltaTime;
    if (Input::IsKeyHeld(GLFW_KEY_S)) velocity -= front * accel * deltaTime;
    if (Input::IsKeyHeld(GLFW_KEY_A)) velocity -= right * accel * deltaTime;
    if (Input::IsKeyHeld(GLFW_KEY_D)) velocity += right * accel * deltaTime;

    if (Input::IsKeyPressed(GLFW_KEY_SPACE) && isGrounded) {
        velocity.y = jumpForce;
        isGrounded = false;
    }
}

void Player::ProcessMouse(float xOffset, float yOffset) {
    camera.ProcessMouse(xOffset, yOffset);
}

bool Player::CheckCollision(glm::vec3 pos, ChunkManager& chunkManager) {
    // Check all blocks inside/intersecting the player's AABB
    glm::vec3 min = pos - glm::vec3(playerSize.x * 0.5f, 0.9f, playerSize.z * 0.5f);
    glm::vec3 max = pos + glm::vec3(playerSize.x * 0.5f, 0.9f, playerSize.z * 0.5f);

    for (int x = (int)std::floor(min.x); x <= (int)std::floor(max.x); x++) {
        for (int y = (int)std::floor(min.y); y <= (int)std::floor(max.y); y++) {
            for (int z = (int)std::floor(min.z); z <= (int)std::floor(max.z); z++) {
                if (chunkManager.GetBlockAtWorldPos(glm::vec3(x, y, z)) != 0) {
                    return true;
                }
            }
        }
    }
    return false;
}
