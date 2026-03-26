#include "Engine.hpp"
#include "../Render/Camera.hpp"
#include "../Render/InstancedMesh.hpp"
#include "../Render/Mesh.hpp"
#include "../Render/Shader.hpp"
#include "../Render/Texture.hpp"
#include "../World/ChunkManager.hpp"
#include "../Entity/Player.hpp"
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
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
}

// Global variables
bool wireframeMode = false;

void Engine::Run() {
  Shader shader("shaders/shader.vs", "shaders/shader.fs");

  ChunkManager chunkManager;
  // Initialize a 4x1x4 area of chunks
  for (int x = -2; x < 2; x++) {
      for (int z = -2; z < 2; z++) {
          chunkManager.AddChunk({x, 0, z});
      }
  }

  Texture texture;
  texture.load("assets/textures/Grass/Grass_01-128x128.png");

  Player player({0.0f, 20.0f, 0.0f});

  float lastTime = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    float currentTime = (float)glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    Input::Update();

    if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
      glfwSetWindowShouldClose(window, true);

    if (Input::IsKeyPressed(GLFW_KEY_SEMICOLON)) {
      if (wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        wireframeMode = false;
      } else if (!wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        wireframeMode = true;
      }
    }

    player.ProcessKeyboard(deltaTime);
    player.ProcessMouse(Input::GetMouseDeltaX(), Input::GetMouseDeltaY());
    player.Update(deltaTime, chunkManager);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Use();

    int w, h = 0;
    glfwGetFramebufferSize(window, &w, &h);
    float aspect = (float)w / (float)h;

    shader.SetMat4("u_view", player.GetCamera().GetViewMatrix());
    shader.SetMat4("u_projection", player.GetCamera().GetProjectionMatrix(aspect));

    texture.bind(0);
    shader.SetInt("u_texture", 0);

    glm::mat4 viewProj = player.GetCamera().GetProjectionMatrix(aspect) * player.GetCamera().GetViewMatrix();
    chunkManager.Render(viewProj);

    glfwSwapBuffers(window);
  }
}

void Engine::Exit() {
  // Cleanup
  glfwDestroyWindow(window);
  glfwTerminate();
}
