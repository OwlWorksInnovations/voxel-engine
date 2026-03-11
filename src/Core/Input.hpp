#pragma once
#include <GLFW/glfw3.h>

class Input {
public:
  static void Init(GLFWwindow *window);
  static void Update();

  static bool IsKeyPressed(int key);
  static bool IsKeyHeld(int key);
  static bool IsKeyReleased(int key);

  static bool IsMouseButtonPressed(int button);
  static bool IsMouseButtonJustPressed(int button);
  static double GetMouseX();
  static double GetMouseY();
  static double GetMouseDeltaX();
  static double GetMouseDeltaY();

private:
  static void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                          int mods);
  static void MouseButtonCallback(GLFWwindow *window, int button, int action,
                                  int mods);
  static void CursorCallback(GLFWwindow *window, double xpos, double ypos);

  static bool currentKeys[GLFW_KEY_LAST];
  static bool previousKeys[GLFW_KEY_LAST];
  static bool previousMouseButtons[GLFW_MOUSE_BUTTON_LAST];
  static bool mouseButtons[GLFW_MOUSE_BUTTON_LAST];

  static double mouseX, mouseY;
  static double lastMouseX, lastMouseY;
  static double deltaX, deltaY;
};
