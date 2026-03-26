#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
  glm::vec3 position;
  float yaw = -90.0f;
  float pitch = 0.0f;
  float speed = 5.0f;
  float sensitivity = 0.1f;
  float fov = 45.0f;

  Camera(glm::vec3 startPos = {0.0f, 0.0f, 3.0f});

  glm::mat4 GetViewMatrix() const;
  glm::mat4 GetProjectionMatrix(float aspectRatio) const;

  void ProcessKeyboard(float deltaTime);
  void ProcessMouse(float xOffset, float yOffset);

  glm::vec3 GetFront() const { return front; }
  glm::vec3 GetRight() const { return right; }
  glm::vec3 GetUp() const { return up; }

private:
  glm::vec3 front = {0.0f, 0.0f, -1.0f};
  glm::vec3 up = {0.0f, 1.0f, 0.0f};
  glm::vec3 right = {1.0f, 0.0f, 0.0f};

  void UpdateVectors();
};
