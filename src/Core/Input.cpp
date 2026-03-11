#include "Input.hpp"
#include <algorithm>

bool Input::currentKeys[GLFW_KEY_LAST] = {};
bool Input::previousKeys[GLFW_KEY_LAST] = {};
bool Input::previousMouseButtons[GLFW_MOUSE_BUTTON_LAST] = {};
bool Input::mouseButtons[GLFW_MOUSE_BUTTON_LAST] = {};
double Input::mouseX = 0, Input::mouseY = 0;
double Input::lastMouseX = 0, Input::lastMouseY = 0;
double Input::deltaX = 0, Input::deltaY = 0;

void Input::Init(GLFWwindow *window) {
  glfwSetKeyCallback(window, KeyCallback);
  glfwSetMouseButtonCallback(window, MouseButtonCallback);
  glfwSetCursorPosCallback(window, CursorCallback);
}

void Input::Update() {
  std::copy(currentKeys, currentKeys + GLFW_KEY_LAST, previousKeys);
  deltaX = mouseX - lastMouseX;
  deltaY = mouseY - lastMouseY;
  lastMouseX = mouseX;
  lastMouseY = mouseY;
  std::copy(mouseButtons, mouseButtons + GLFW_MOUSE_BUTTON_LAST,
            previousMouseButtons);
}

void Input::KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                        int mods) {
  if (key < 0 || key >= GLFW_KEY_LAST)
    return;
  currentKeys[key] = (action != GLFW_RELEASE);
}

void Input::MouseButtonCallback(GLFWwindow *window, int button, int action,
                                int mods) {
  if (button < 0 || button >= GLFW_MOUSE_BUTTON_LAST)
    return;
  mouseButtons[button] = (action != GLFW_RELEASE);
}

void Input::CursorCallback(GLFWwindow *window, double xpos, double ypos) {
  mouseX = xpos;
  mouseY = ypos;
}

bool Input::IsKeyPressed(int key) {
  return currentKeys[key] && !previousKeys[key];
}
bool Input::IsKeyHeld(int key) { return currentKeys[key]; }
bool Input::IsKeyReleased(int key) {
  return !currentKeys[key] && previousKeys[key];
}

bool Input::IsMouseButtonPressed(int button) {
  if (button < 0 || button >= GLFW_MOUSE_BUTTON_LAST) return false;
  return mouseButtons[button];
}
bool Input::IsMouseButtonJustPressed(int button) {
  if (button < 0 || button >= GLFW_MOUSE_BUTTON_LAST) return false;
  return mouseButtons[button] && !previousMouseButtons[button];
}
double Input::GetMouseX() { return mouseX; }
double Input::GetMouseY() { return mouseY; }
double Input::GetMouseDeltaX() { return deltaX; }
double Input::GetMouseDeltaY() { return deltaY; }
