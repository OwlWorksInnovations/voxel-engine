#include "Engine.hpp"
#include "../Render/Camera.hpp"
#include "../Render/Mesh.hpp"
#include "../Render/Shader.hpp"
#include "../Render/Texture.hpp"
#include "../World/Raycast.hpp"
#include "../World/World.hpp"
#include "../World/Block.hpp"
#include "Input.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>

GLFWwindow *Engine::window = nullptr;
Shader *Engine::shader = nullptr;
Camera *Engine::camera = nullptr;

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
  glEnable(GL_DEPTH_TEST);

  shader = new Shader("shaders/shader.vs", "shaders/shader.fs");

  Input::Init(window);

  camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Engine::Run() {
  float lastFrame = 0.0f;
  // Example

  // Mesh
  std::vector<Vertex> vertices = {
      {-0.5f, 0.5f, 0.0f, 0.0f, 1.0f},  // top left
      {0.5f, 0.5f, 0.0f, 1.0f, 1.0f},   // top right
      {0.5f, -0.5f, 0.0f, 1.0f, 0.0f},  // bottom right
      {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f}, // bottom left
  };
  std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};
  Mesh mesh;
  mesh.SetData(vertices, indices);

  // Texture
  Texture texture("assets/textures/Grass/Grass_01-128x128.png");

  // Chunk
  // Chunk chunk(glm::ivec3(0, 0, 0));
  // chunk.Generate(1337);

  // World
  World world(1337, 16);

  while (!glfwWindowShouldClose(window)) {
    // Update input before wiping state
    Input::Update();
    float currentFrame = glfwGetTime();
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    glfwPollEvents();

    // Input
    if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
      glfwSetWindowShouldClose(window, true);

    // Camera
    if (Input::IsKeyHeld(GLFW_KEY_W))
      camera->Move(CameraMovement::FORWARD, deltaTime);
    if (Input::IsKeyHeld(GLFW_KEY_S))
      camera->Move(CameraMovement::BACKWARD, deltaTime);
    if (Input::IsKeyHeld(GLFW_KEY_A))
      camera->Move(CameraMovement::LEFT, deltaTime);
    if (Input::IsKeyHeld(GLFW_KEY_D))
      camera->Move(CameraMovement::RIGHT, deltaTime);
    if (Input::IsKeyHeld(GLFW_KEY_SPACE))
      camera->Move(CameraMovement::UP, deltaTime);
    if (Input::IsKeyHeld(GLFW_KEY_LEFT_SHIFT))
      camera->Move(CameraMovement::DOWN, deltaTime);
    camera->Rotate(Input::GetMouseDeltaX(), -Input::GetMouseDeltaY());

    // Clear screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Camera
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = camera->GetProjectionMatrix((float)w / (float)h);
    glm::mat4 model = glm::mat4(1.0f);

    // Update world with play position (must be before block operations)
    world.Update(camera->position);

    // Block breaking and placing
    RaycastResult hit = Raycast(world, camera->position, camera->front, 10.0f);

    if (Input::IsMouseButtonJustPressed(GLFW_MOUSE_BUTTON_LEFT) && hit.hit) {
      world.SetBlockVoxel(hit.blockPos, BlockType::Air);
    }

    if (Input::IsMouseButtonJustPressed(GLFW_MOUSE_BUTTON_RIGHT) && hit.hit) {
      glm::ivec3 placePos = hit.blockPos + hit.normal;
      if (IsAir(world.GetBlockVoxel(placePos))) {
        world.SetBlockVoxel(placePos, BlockType::Grass);
      }
    }

    // Shader uniforms
    shader->Use();
    texture.Bind(0);
    shader->SetInt("texture1", 0);
    shader->SetMat4("model", model);
    shader->SetMat4("view", view);
    shader->SetMat4("projection", projection);

    // Update world with play position
    world.Update(camera->position);

    // Draw calls
    // mesh.Draw();
    // chunk.Draw();
    world.Draw(*shader);

    glfwSwapBuffers(window);
  }
}

void Engine::Shutdown() {
  delete shader;
  delete camera;
  glfwDestroyWindow(window);
  glfwTerminate();
}
