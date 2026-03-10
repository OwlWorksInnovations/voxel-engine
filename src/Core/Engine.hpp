#pragma once
#include "Shader.hpp"
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <string>

class Engine {
public:
  static void InitWindow(int width, int height, const std::string &windowTitle);
  static void Run();
  static void Shutdown();

private:
  static GLFWwindow *window;
  static Shader *shader;
};
