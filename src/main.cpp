#include "Chunk.h"
#include "ECS.h"
#include "Mesh.h"
#include "Perlin.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

// Global variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int screenWidth = 1920;
int screenHeight = 1080;

// Entity
struct Entity {
  std::string name;
  int id;
};
std::vector<Entity> entities;
int selectedEntity = -1;
Registry registry;

// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 10.0f, 17.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;

// Other
bool wireframeMode = false;
bool sceneViewFocused = false;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  (void)window;
  screenWidth = width;
  screenHeight = height;
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  (void)window;
  if (!sceneViewFocused)
    return;

  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  const float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  (void)window;
  (void)xoffset;
  (void)yoffset;
  // reserved for zoom later
}

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    sceneViewFocused = false;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  if (!sceneViewFocused)
    return;

  float cameraSpeed = 2.5f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos +=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

  // Wireframe toggle (debounced)
  static bool lastSemicolon = false;
  bool currentSemicolon = glfwGetKey(window, GLFW_KEY_SEMICOLON) == GLFW_PRESS;
  if (currentSemicolon && !lastSemicolon)
    wireframeMode = !wireframeMode;
  lastSemicolon = currentSemicolon;
}

int main() {
  // Create window
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
  GLFWwindow *window =
      glfwCreateWindow(screenWidth, screenHeight, "Voxel Engine", NULL, NULL);
  if (window == NULL) {
    std::cout << "Window failed to create!" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD!" << std::endl;
    return -1;
  }
  // Set window size
  glViewport(0, 0, screenWidth, screenHeight);
  glEnable(GL_DEPTH_TEST);

  // Load shaders into shader class
  Shader ourShader("shader.vs", "shader.fs");

  // Set up ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  // Create framebuffer
  unsigned int FBO;
  glGenFramebuffers(1, &FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  // Create texture to render into
  unsigned int renderTexture;
  glGenTextures(1, &renderTexture);
  glBindTexture(GL_TEXTURE_2D, renderTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         renderTexture, 0);

  // Create renderbuffer for depth
  unsigned int RBO;
  glGenRenderbuffers(1, &RBO);
  glBindRenderbuffer(GL_RENDERBUFFER, RBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth,
                        screenHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, RBO);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Load and create texture
  Texture containerTexture("assets/textures/container.jpg", GL_RGB, 0,
                           GL_REPEAT, GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR,
                           GL_LINEAR);
  ourShader.use();
  ourShader.setInt("texture1", 0);

  // Default cube mesh
  Mesh cubeMesh;
  cubeMesh.create(DefaultMeshes::cubeVertices, DefaultMeshes::cubeIndices);

  // Perlin noise
  ChunkManager chunkManager;
  PerlinNoise perlin;
  perlin.setMapSize(16, 16, 16.0f);
  perlin.generatePerlinNoise();
  perlin.generateVoxelData(chunkManager.chunk, perlin.sampledValues, 15);
  chunkManager.createChunk();
  perlin.generateImage("perlinnoise.png");

  // Render loop
  while (!glfwWindowShouldClose(window)) {

    // deltaTime
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Setup
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

    process_input(window);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (wireframeMode) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Bind framebuffer and render scene into it
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Freecam
    ourShader.use();
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f,
        100.0f);
    ourShader.setMat4("view", view);
    ourShader.setMat4("projection", projection);

    // Set textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, containerTexture.textureID);

    // Draw chunks
    for (Chunk &chunk : chunkManager.chunks) {
      glm::mat4 model = glm::mat4(1.0f);
      ourShader.setMat4("model", model);
      glBindVertexArray(chunk.VAO);
      glDrawElements(GL_TRIANGLES, chunk.indexCount, GL_UNSIGNED_INT, 0);
    }

    // Draw entities
    for (MeshRenderer &mr : registry.meshRenderers) {
      Transform *transform = registry.getTransform(mr.entityID);
      if (!transform)
        continue;

      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, transform->position);
      model = glm::rotate(model, glm::radians(transform->rotation.x),
                          glm::vec3(1, 0, 0));
      model = glm::rotate(model, glm::radians(transform->rotation.y),
                          glm::vec3(0, 1, 0));
      model = glm::rotate(model, glm::radians(transform->rotation.z),
                          glm::vec3(0, 0, 1));
      model = glm::scale(model, transform->scale);
      ourShader.setMat4("model", model);

      glBindTexture(GL_TEXTURE_2D, mr.textureID);
      glBindVertexArray(mr.VAO);
      glDrawElements(GL_TRIANGLES, cubeMesh.indexCount, GL_UNSIGNED_INT, 0);
    }

    // Unbind framebuffer
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Hierarchy panel
    ImGui::Begin("Hierarchy");
    if (ImGui::BeginPopupContextWindow("HierarchyContext")) {
      if (ImGui::MenuItem("Create Entity")) {
        Entity e;
        int id = registry.createEntity();
        e.name = "Entity " + std::to_string(id);
        e.id = id;
        entities.push_back(e);
        registry.addComponent<Transform>(id);
      }
      ImGui::EndPopup();
    }
    for (int i = 0; i < (int)chunkManager.chunks.size(); i++) {
      ImGui::Text("Chunk %d", i);
    }
    for (int i = 0; i < (int)entities.size(); i++) {
      bool selected = (selectedEntity == i);
      if (ImGui::Selectable(entities[i].name.c_str(), selected))
        selectedEntity = i;
    }
    ImGui::End();

    // Scene view panel (UV flipped to correct OpenGL/ImGui Y-axis mismatch)
    ImGui::Begin("Scene");
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) {
      sceneViewFocused = true;
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      firstMouse = true;
    }
    ImGui::Image((ImTextureID)(intptr_t)renderTexture,
                 ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

    // Properties panel
    ImGui::Begin("Properties");
    if (selectedEntity >= 0 && selectedEntity < (int)entities.size()) {
      Entity &e = entities[selectedEntity];

      // Name field
      char nameBuf[128];
      strncpy(nameBuf, e.name.c_str(), sizeof(nameBuf));
      if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf)))
        e.name = nameBuf;

      ImGui::Separator();

      // Transform component
      Transform *transform = registry.getTransform(e.id);
      if (transform) {
        if (ImGui::CollapsingHeader("Transform",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::DragFloat3("Position", &transform->position.x, 0.1f);
          ImGui::DragFloat3("Rotation", &transform->rotation.x, 0.1f);
          ImGui::DragFloat3("Scale", &transform->scale.x, 0.1f);
        }
      }

      // MeshRenderer component
      MeshRenderer *mesh = nullptr;
      for (MeshRenderer &mr : registry.meshRenderers)
        if (mr.entityID == e.id) {
          mesh = &mr;
          break;
        }
      if (mesh) {
        if (ImGui::CollapsingHeader("Mesh Renderer",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Text("VAO: %u", mesh->VAO);
          ImGui::Text("Texture ID: %u", mesh->textureID);
        }
      }

      ImGui::Separator();

      // Add component button
      if (ImGui::Button("Add Component"))
        ImGui::OpenPopup("AddComponent");
      if (ImGui::BeginPopup("AddComponent")) {
        if (!transform && ImGui::MenuItem("Transform"))
          registry.addComponent<Transform>(e.id);
        if (!mesh && ImGui::MenuItem("Mesh Renderer"))
          registry.addMeshRenderer(e.id, cubeMesh.VAO,
                                   containerTexture.textureID);
        ImGui::EndPopup();
      }
    } else {
      ImGui::Text("No entity selected");
    }
    ImGui::End();

    // ImGui rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  cubeMesh.destroy();
  glDeleteFramebuffers(1, &FBO);
  glDeleteRenderbuffers(1, &RBO);
  glDeleteTextures(1, &renderTexture);
  glfwTerminate();
  return 0;
}
