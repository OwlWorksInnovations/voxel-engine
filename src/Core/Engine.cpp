#include "Engine.hpp"
#include <GLFW/glfw3.h>

GLFWwindow *Engine::window = nullptr;

void Engine::InitWindow(int width, int height, const std::string &windowTitle) {
  glfwInit();
  window =
      glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void Engine::Run() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    glfwSwapBuffers(window);
  }
}

void Engine::Shutdown() {
  glfwDestroyWindow(window);
  glfwTerminate();
}
