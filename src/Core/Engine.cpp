#include "Engine.hpp"
#include "../Render/Mesh.hpp"
#include "../Render/Shader.hpp"
#include "../Render/Texture.hpp"
#include "Input.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>

GLFWwindow *Engine::window = nullptr;
Shader *Engine::shader = nullptr;

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

  shader = new Shader("shaders/shader.vs", "shaders/shader.fs");
  Input::Init(window);
}

void Engine::Run() {
  // simple quad
  std::vector<Vertex> vertices = {
      {-0.5f, 0.5f, 0.0f, 0.0f, 1.0f},  // top left
      {0.5f, 0.5f, 0.0f, 1.0f, 1.0f},   // top right
      {0.5f, -0.5f, 0.0f, 1.0f, 0.0f},  // bottom right
      {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f}, // bottom left
  };

  std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};

  Mesh mesh;
  mesh.SetData(vertices, indices);

  Texture texture("assets/textures/Grass/Grass_01-128x128.png");

  while (!glfwWindowShouldClose(window)) {
    // Update input before wiping state
    Input::Update();
    glfwPollEvents();

    // Input
    if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
      glfwSetWindowShouldClose(window, true);

    // Clear screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader->Use();
    texture.Bind(0);
    shader->SetInt("texture1", 0);
    mesh.Draw();

    glfwSwapBuffers(window);
  }
}

void Engine::Shutdown() {
  delete shader;
  glfwDestroyWindow(window);
  glfwTerminate();
}
