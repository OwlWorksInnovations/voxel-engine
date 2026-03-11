#pragma once
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <vector>

enum class KeyState { None, Pressed, Held, Released };

struct KeyEvent {
  int key;
  KeyState state;
};

class Input {
public:
  static void Init(GLFWwindow *window);
  static void Update();

  static bool IsKeyPressed(int key);
  static bool IsKeyHeld(int key);
  static bool IsKeyReleased(int key);

  static const std::vector<KeyEvent> &GetEventQueue();

private:
  static void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                          int mods);

  static std::unordered_map<int, KeyState> keyStates;
  static std::vector<KeyEvent> eventQueue;
  static std::vector<KeyEvent> frameQueue;
};
