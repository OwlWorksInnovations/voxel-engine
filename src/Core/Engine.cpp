#include "Engine.hpp"
#include "../Render/Mesh.hpp"
#include "../Render/Shader.hpp"
#include "../Render/Texture.hpp"
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
  // Shader
  Shader shader("shaders/shader.vs", "shaders/shader.fs");

  // Mesh
  Mesh mesh;

  std::vector<Vertex> vertices = {
      {-0.5f, 0.5f, 0.0f, 0.0f, 1.0f},  // top-left
      {0.5f, 0.5f, 0.0f, 1.0f, 1.0f},   // top-right
      {0.5f, -0.5f, 0.0f, 1.0f, 0.0f},  // bottom-right
      {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f}, // bottom-left
  };

  std::vector<unsigned int> indices = {
      0, 1, 2, // first triangle
      0, 2, 3  // second triangle
  };

  mesh.SetData(vertices, indices);

  // Texture
  Texture texture;
  texture.load("assets/textures/Bricks/Bricks_01-128x128.png");

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    // Shader
    shader.Use();

    // Input
    Input::Update();

    // Clear the screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (Input::IsKeyPressed(GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(window, true);
    }

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    shader.SetMat4("u_model", model);
    shader.SetMat4("u_view", view);
    shader.SetMat4("u_projection", projection);

    // Texture
    texture.bind(0);
    shader.SetInt("u_texture", 0);

    mesh.Draw();

    // Swap front and back buffers
    glfwSwapBuffers(window);
  }
}

void Engine::Exit() {
  // Cleanup
  glfwDestroyWindow(window);
  glfwTerminate();
}
