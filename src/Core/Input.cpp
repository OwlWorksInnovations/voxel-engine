#include "Input.hpp"

std::unordered_map<int, KeyState> Input::keyStates;
std::vector<KeyEvent> Input::eventQueue;
std::vector<KeyEvent> Input::frameQueue;

void Input::Init(GLFWwindow *window) {
  glfwSetKeyCallback(window, KeyCallback);
}

void Input::Update() {
  glfwPollEvents();

  // Age all Pressed → Held, Released → None
  for (auto &[key, state] : keyStates) {
    if (state == KeyState::Pressed)
      state = KeyState::Held;
    if (state == KeyState::Released)
      state = KeyState::None;
  }

  // Snapshot and flush the queue
  frameQueue = eventQueue;
  eventQueue.clear();

  // Apply new events to state map
  for (const auto &event : frameQueue) {
    keyStates[event.key] = event.state;
  }
}

void Input::KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                        int mods) {
  KeyState state = KeyState::None;

  if (action == GLFW_PRESS)
    state = KeyState::Pressed;
  if (action == GLFW_RELEASE)
    state = KeyState::Released;
  if (action == GLFW_REPEAT)
    state = KeyState::Held;

  eventQueue.push_back({key, state});
}

bool Input::IsKeyPressed(int key) {
  auto it = keyStates.find(key);
  return it != keyStates.end() && it->second == KeyState::Pressed;
}

bool Input::IsKeyHeld(int key) {
  auto it = keyStates.find(key);
  return it != keyStates.end() && it->second == KeyState::Held;
}

bool Input::IsKeyReleased(int key) {
  auto it = keyStates.find(key);
  return it != keyStates.end() && it->second == KeyState::Released;
}

const std::vector<KeyEvent> &Input::GetEventQueue() { return frameQueue; }
