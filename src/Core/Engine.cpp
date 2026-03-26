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
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

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

  // Initialize ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

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

    // Draw Crosshair using ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImVec2 center = ImVec2((float)w * 0.5f, (float)h * 0.5f);
    ImGui::GetBackgroundDrawList()->AddCircle(center, 5.0f, IM_COL32(255, 255, 255, 200), 16, 2.0f);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(center.x - 10, center.y), ImVec2(center.x + 10, center.y), IM_COL32(255, 255, 255, 200), 2.0f);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(center.x, center.y - 10), ImVec2(center.x, center.y + 10), IM_COL32(255, 255, 255, 200), 2.0f);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }
}

void Engine::Exit() {
  // Cleanup
  glfwDestroyWindow(window);
  glfwTerminate();
}
