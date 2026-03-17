#pragma once
#include <glad/glad.h>

#include <GLFW/glfw3.h>

class Engine {
public:
  static void InitWindow(int width, int height, const char *title);
  static void Run();
  static void Exit();

private:
  static GLFWwindow *window;
};
