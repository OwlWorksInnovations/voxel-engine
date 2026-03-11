#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : position(position), worldUp(up), yaw(yaw), pitch(pitch), moveSpeed(10.0f),
      mouseSensitivity(0.1f), fov(45.0f) {
  front = glm::vec3(0.0f, 0.0f, -1.0f);
  UpdateVectors();
}

glm::mat4 Camera::GetViewMatrix() {
  return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) {
  return glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 1000.0f);
}

void Camera::Move(CameraMovement direction, float deltaTime) {
  float velocity = moveSpeed * deltaTime;
  switch (direction) {
  case CameraMovement::FORWARD:
    position += front * velocity;
    break;
  case CameraMovement::BACKWARD:
    position -= front * velocity;
    break;
  case CameraMovement::LEFT:
    position -= right * velocity;
    break;
  case CameraMovement::RIGHT:
    position += right * velocity;
    break;
  case CameraMovement::UP:
    position += worldUp * velocity;
    break;
  case CameraMovement::DOWN:
    position -= worldUp * velocity;
    break;
  }
}

void Camera::Rotate(float xOffset, float yOffset) {
  xOffset *= mouseSensitivity;
  yOffset *= mouseSensitivity;

  yaw += xOffset;
  pitch += yOffset;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  UpdateVectors();
}

void Camera::Zoom(float yOffset) {
  fov -= yOffset;
  if (fov < 1.0f)
    fov = 1.0f;
  if (fov > 90.0f)
    fov = 90.0f;
}

void Camera::UpdateVectors() {
  glm::vec3 newFront;
  newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  newFront.y = sin(glm::radians(pitch));
  newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  front = glm::normalize(newFront);
  right = glm::normalize(glm::cross(front, worldUp));
  up = glm::normalize(glm::cross(right, front));
}
