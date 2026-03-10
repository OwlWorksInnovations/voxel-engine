#include "Chunk.h"
#include "ECS.h"
#include "Mesh.h"
#include "Perlin.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
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

// ECS Registry
Registry registry;

// Camera
glm::vec3 cameraPos = glm::vec3(8.0f, 20.0f, 25.0f);
glm::vec3 cameraFront = glm::normalize(glm::vec3(0.0f, -0.5f, -1.0f));
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;
float pitch = -30.0f;
float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;

// Other
bool wireframeMode = false;
bool mouseCaptured = true;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  (void)window;
  screenWidth = width;
  screenHeight = height;
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  (void)window;
  if (!mouseCaptured)
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

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    mouseCaptured = false;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    mouseCaptured = true;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    firstMouse = true;
  }

  if (!mouseCaptured)
    return;

  float cameraSpeed = 5.0f * deltaTime;
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
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD!" << std::endl;
    return -1;
  }
  
  glEnable(GL_DEPTH_TEST);

  // Load shaders
  Shader ourShader("shader.vs", "shader.fs");

  // Load and create textures
  Texture containerTexture("assets/textures/container.jpg", GL_RGB, 0,
                           GL_REPEAT, GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR,
                           GL_LINEAR);
  Texture awesomefaceTexture("assets/textures/awesomeface.png", GL_RGBA, 0,
                             GL_REPEAT, GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR,
                             GL_LINEAR);

  ourShader.use();
  ourShader.setInt("texture1", 0);

  // Default cube mesh
  Mesh cubeMesh;
  cubeMesh.create(DefaultMeshes::cubeVertices, DefaultMeshes::cubeIndices);

  // Perlin noise terrain
  ChunkManager chunkManager;
  PerlinNoise perlin;
  float maxHeight = 15.0f;
  perlin.setMapSize(16, 16, 16.0f);
  perlin.generatePerlinNoise();
  perlin.generateVoxelData(chunkManager.chunk, perlin.sampledValues, maxHeight);
  chunkManager.createChunk();

  // Create cube entity on top of terrain at (8, 8)
  int cubeX = 8;
  int cubeZ = 8;
  int terrainHeight = (perlin.sampledValues[cubeX * 16 + cubeZ] + 1.0f) / 2.0f * maxHeight;
  
  int entityId = registry.createEntity();
  registry.addComponent<Transform>(entityId);
  Transform* transform = registry.getTransform(entityId);
  transform->position = glm::vec3(cubeX, terrainHeight + 1.5f, cubeZ);
  transform->scale = glm::vec3(1.0f, 1.0f, 1.0f);
  
  registry.addMeshRenderer(entityId, cubeMesh.VAO, awesomefaceTexture.textureID);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    process_input(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (wireframeMode)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    ourShader.use();
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f,
        100.0f);
    ourShader.setMat4("view", view);
    ourShader.setMat4("projection", projection);

    // Draw chunks
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, containerTexture.textureID);
    for (Chunk &chunk : chunkManager.chunks) {
      glm::mat4 model = glm::mat4(1.0f);
      ourShader.setMat4("model", model);
      glBindVertexArray(chunk.VAO);
      glDrawElements(GL_TRIANGLES, chunk.indexCount, GL_UNSIGNED_INT, 0);
    }

    // Draw entities
    for (MeshRenderer &mr : registry.meshRenderers) {
      Transform *t = registry.getTransform(mr.entityID);
      if (!t) continue;

      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, t->position);
      model = glm::rotate(model, glm::radians(t->rotation.x), glm::vec3(1, 0, 0));
      model = glm::rotate(model, glm::radians(t->rotation.y), glm::vec3(0, 1, 0));
      model = glm::rotate(model, glm::radians(t->rotation.z), glm::vec3(0, 0, 1));
      model = glm::scale(model, t->scale);
      ourShader.setMat4("model", model);

      glBindTexture(GL_TEXTURE_2D, mr.textureID);
      glBindVertexArray(mr.VAO);
      glDrawElements(GL_TRIANGLES, cubeMesh.indexCount, GL_UNSIGNED_INT, 0);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  cubeMesh.destroy();
  glfwTerminate();
  return 0;
}
