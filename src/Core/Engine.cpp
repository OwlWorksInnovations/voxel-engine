#include "Engine.hpp"
#include "Input.hpp"
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <iostream>

GLFWwindow *Engine::window = nullptr;

void Engine::InitWindow(int width, int height, const char *title) {
  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(width, height, title, NULL, NULL);

  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return;
  }

  Input::Init(window);

  glEnable(GL_DEPTH_TEST);
}

void Engine::Run() {
  // Main loop
  while (!glfwWindowShouldClose(window)) {
    // Input
    Input::Update();

    // Clear the screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (Input::IsKeyPressed(GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(window, true);
    }

    // Swap front and back buffers
    glfwSwapBuffers(window);
  }
}

void Engine::Exit() {
  // Cleanup
  glfwDestroyWindow(window);
  glfwTerminate();
}
