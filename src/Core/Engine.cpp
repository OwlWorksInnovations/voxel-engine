#include "Engine.hpp"
#include "Input.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>

GLFWwindow *Engine::window = nullptr;

void Engine::InitWindow(int width, int height, const std::string &windowTitle) {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  window =
      glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfwTerminate();
    throw std::runtime_error("Failed to initialize GLAD");
  }

  Input::Init(window);
}

void Engine::Run() {
  while (!glfwWindowShouldClose(window)) {
    // Update input before wiping state
    Input::Update();
    glfwPollEvents();

    if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
      glfwSetWindowShouldClose(window, true);

    glfwSwapBuffers(window);
  }
}

void Engine::Shutdown() {
  glfwDestroyWindow(window);
  glfwTerminate();
}
