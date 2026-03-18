#include "Camera.hpp"
#include "../Core/Input.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 startPos) : position(startPos) { UpdateVectors(); }

glm::mat4 Camera::GetViewMatrix() const {
  return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const {
  return glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
}

void Camera::ProcessKeyboard(float deltaTime) {
  float velocity = speed * deltaTime;
  if (Input::IsKeyPressed(GLFW_KEY_W) || Input::IsKeyHeld(GLFW_KEY_W))
    position += front * velocity;
  if (Input::IsKeyPressed(GLFW_KEY_S) || Input::IsKeyHeld(GLFW_KEY_S))
    position -= front * velocity;
  if (Input::IsKeyPressed(GLFW_KEY_A) || Input::IsKeyHeld(GLFW_KEY_A))
    position -= right * velocity;
  if (Input::IsKeyPressed(GLFW_KEY_D) || Input::IsKeyHeld(GLFW_KEY_D))
    position += right * velocity;
  if (Input::IsKeyPressed(GLFW_KEY_SPACE) || Input::IsKeyHeld(GLFW_KEY_SPACE))
    position += up * velocity;
  if (Input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL) ||
      Input::IsKeyHeld(GLFW_KEY_LEFT_CONTROL))
    position -= up * velocity;
}

void Camera::ProcessMouse(float xOffset, float yOffset) {
  yaw += xOffset * sensitivity;
  pitch += yOffset * sensitivity;
  pitch = std::clamp(pitch, -89.0f, 89.0f); // prevent gimbal flip
  UpdateVectors();
}

void Camera::UpdateVectors() {
  glm::vec3 f;
  f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  f.y = sin(glm::radians(pitch));
  f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  front = glm::normalize(f);
  right = glm::normalize(glm::cross(front, {0.0f, 1.0f, 0.0f}));
  up = glm::normalize(glm::cross(right, front));
}
