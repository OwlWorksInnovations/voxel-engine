#include "Input.hpp"

std::unordered_map<int, KeyState> Input::keyStates;
std::vector<KeyEvent> Input::eventQueue;
std::vector<KeyEvent> Input::frameQueue;
GLFWwindow *Input::window = nullptr; // add
float Input::mouseDeltaX = 0.0f;     // add
float Input::mouseDeltaY = 0.0f;     // add

static double lastX = 0;
static double lastY = 0;
static bool firstMouse = true;

void Input::Init(GLFWwindow *w) {
  window = w; // store it
  glfwSetKeyCallback(window, KeyCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // capture mouse
}

void Input::Update() {
  glfwPollEvents();

  // Age Pressed → Held, Released → None
  for (auto &[key, state] : keyStates) {
    if (state == KeyState::Pressed)
      state = KeyState::Held;
    if (state == KeyState::Released)
      state = KeyState::None;
  }

  // Snapshot and flush the callback queue
  frameQueue = eventQueue;
  eventQueue.clear();

  // Mouse delta
  double x, y;
  glfwGetCursorPos(window, &x, &y);
  if (firstMouse) {
    lastX = x;
    lastY = y;
    firstMouse = false;
  }
  mouseDeltaX = (float)(x - lastX);
  mouseDeltaY = (float)(lastY - y);
  lastX = x;
  lastY = y;

  // Apply new events to state map
  for (const auto &event : frameQueue)
    keyStates[event.key] = event.state;
}

void Input::KeyCallback(GLFWwindow *w, int key, int scancode, int action,
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

bool Input::IsMouseButtonPressed(int button) {
    static std::unordered_map<int, bool> lastState;
    bool current = glfwGetMouseButton(window, button) == GLFW_PRESS;
    bool pressed = current && !lastState[button];
    lastState[button] = current;
    return pressed;
}

bool Input::IsMouseButtonHeld(int button) {
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

float Input::GetMouseDeltaX() { return mouseDeltaX; }
float Input::GetMouseDeltaY() { return mouseDeltaY; }

const std::vector<KeyEvent> &Input::GetEventQueue() { return frameQueue; }
