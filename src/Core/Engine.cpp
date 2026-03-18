#include "Engine.hpp"
#include "../Render/Camera.hpp"
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

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  Input::Init(window);

  glEnable(GL_DEPTH_TEST);
}

void Engine::Run() {
  Shader shader("shaders/shader.vs", "shaders/shader.fs");

  Mesh mesh;
  std::vector<Vertex> vertices = {
      // Back face
      {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f},
      {0.5f, -0.5f, -0.5f, 1.0f, 0.0f},
      {0.5f, 0.5f, -0.5f, 1.0f, 1.0f},
      {-0.5f, 0.5f, -0.5f, 0.0f, 1.0f},
      // Front face
      {-0.5f, -0.5f, 0.5f, 0.0f, 0.0f},
      {0.5f, -0.5f, 0.5f, 1.0f, 0.0f},
      {0.5f, 0.5f, 0.5f, 1.0f, 1.0f},
      {-0.5f, 0.5f, 0.5f, 0.0f, 1.0f},
      // Left face
      {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f},
      {-0.5f, -0.5f, 0.5f, 1.0f, 0.0f},
      {-0.5f, 0.5f, 0.5f, 1.0f, 1.0f},
      {-0.5f, 0.5f, -0.5f, 0.0f, 1.0f},
      // Right face
      {0.5f, -0.5f, 0.5f, 0.0f, 0.0f},
      {0.5f, -0.5f, -0.5f, 1.0f, 0.0f},
      {0.5f, 0.5f, -0.5f, 1.0f, 1.0f},
      {0.5f, 0.5f, 0.5f, 0.0f, 1.0f},
      // Bottom face
      {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f},
      {0.5f, -0.5f, -0.5f, 1.0f, 0.0f},
      {0.5f, -0.5f, 0.5f, 1.0f, 1.0f},
      {-0.5f, -0.5f, 0.5f, 0.0f, 1.0f},
      // Top face
      {-0.5f, 0.5f, 0.5f, 0.0f, 0.0f},
      {0.5f, 0.5f, 0.5f, 1.0f, 0.0f},
      {0.5f, 0.5f, -0.5f, 1.0f, 1.0f},
      {-0.5f, 0.5f, -0.5f, 0.0f, 1.0f},
  };

  std::vector<unsigned int> indices = {
      0,  1,  2,  0,  2,  3,  // back
      4,  5,  6,  4,  6,  7,  // front
      8,  9,  10, 8,  10, 11, // left
      12, 13, 14, 12, 14, 15, // right
      16, 17, 18, 16, 18, 19, // bottom
      20, 21, 22, 20, 22, 23, // top
  };
  mesh.SetData(vertices, indices);

  Texture texture;
  texture.load("assets/textures/Bricks/Bricks_01-128x128.png");

  Camera camera({0.0f, 0.0f, 3.0f});

  float lastTime = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    float currentTime = (float)glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    Input::Update();

    if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
      glfwSetWindowShouldClose(window, true);

    camera.ProcessKeyboard(deltaTime);
    camera.ProcessMouse(Input::GetMouseDeltaX(), Input::GetMouseDeltaY());

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Use();

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    float aspect = (float)w / (float)h;

    shader.SetMat4("u_model", glm::mat4(1.0f));
    shader.SetMat4("u_view", camera.GetViewMatrix());
    shader.SetMat4("u_projection", camera.GetProjectionMatrix(aspect));

    texture.bind(0);
    shader.SetInt("u_texture", 0);
    mesh.Draw();

    glfwSwapBuffers(window);
  }
}

void Engine::Exit() {
  // Cleanup
  glfwDestroyWindow(window);
  glfwTerminate();
}
