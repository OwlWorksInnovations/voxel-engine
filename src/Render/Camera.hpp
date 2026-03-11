#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

class Camera {
public:
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 worldUp;

  float yaw;
  float pitch;
  float moveSpeed;
  float mouseSensitivity;
  float fov;

  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f,
         float pitch = 0.0f);

  glm::mat4 GetViewMatrix();
  glm::mat4 GetProjectionMatrix(float aspectRatio);

  void Move(CameraMovement direction, float deltaTime);
  void Rotate(float xOffset, float yOffset);
  void Zoom(float yOffset);

private:
  void UpdateVectors();
};
