#include "Shader.h"
#include "Texture.h"
#include "ecs/ecs.h"
#include "glm/detail/qualifier.hpp"
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

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
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
  // reserved for zoom later
}

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

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
}

int main() {
  // Create window
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *window = glfwCreateWindow(800, 600, "Voxel Engine", NULL, NULL);
  if (window == NULL) {
    std::cout << "Window failed to create!" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD!" << std::endl;
    return -1;
  }
  // Set window size
  glViewport(0, 0, 800, 600);
  glEnable(GL_DEPTH_TEST);

  // Load shaders into shader class
  Shader ourShader("shader.vs", "shader.fs");

  // Cube vertices, indices, and positions
  float vertices[] = {
      // positions        // tex coords
      // Front face
      -0.5f,
      -0.5f,
      0.5f,
      0.0f,
      0.0f,
      0.5f,
      -0.5f,
      0.5f,
      1.0f,
      0.0f,
      0.5f,
      0.5f,
      0.5f,
      1.0f,
      1.0f,
      -0.5f,
      0.5f,
      0.5f,
      0.0f,
      1.0f,
      // Back face
      -0.5f,
      -0.5f,
      -0.5f,
      0.0f,
      0.0f,
      0.5f,
      -0.5f,
      -0.5f,
      1.0f,
      0.0f,
      0.5f,
      0.5f,
      -0.5f,
      1.0f,
      1.0f,
      -0.5f,
      0.5f,
      -0.5f,
      0.0f,
      1.0f,
      // Left face
      -0.5f,
      -0.5f,
      -0.5f,
      0.0f,
      0.0f,
      -0.5f,
      -0.5f,
      0.5f,
      1.0f,
      0.0f,
      -0.5f,
      0.5f,
      0.5f,
      1.0f,
      1.0f,
      -0.5f,
      0.5f,
      -0.5f,
      0.0f,
      1.0f,
      // Right face
      0.5f,
      -0.5f,
      -0.5f,
      0.0f,
      0.0f,
      0.5f,
      -0.5f,
      0.5f,
      1.0f,
      0.0f,
      0.5f,
      0.5f,
      0.5f,
      1.0f,
      1.0f,
      0.5f,
      0.5f,
      -0.5f,
      0.0f,
      1.0f,
      // Top face
      -0.5f,
      0.5f,
      -0.5f,
      0.0f,
      0.0f,
      -0.5f,
      0.5f,
      0.5f,
      1.0f,
      0.0f,
      0.5f,
      0.5f,
      0.5f,
      1.0f,
      1.0f,
      0.5f,
      0.5f,
      -0.5f,
      0.0f,
      1.0f,
      // Bottom face
      -0.5f,
      -0.5f,
      -0.5f,
      0.0f,
      0.0f,
      -0.5f,
      -0.5f,
      0.5f,
      1.0f,
      0.0f,
      0.5f,
      -0.5f,
      0.5f,
      1.0f,
      1.0f,
      0.5f,
      -0.5f,
      -0.5f,
      0.0f,
      1.0f,
  };

  unsigned int indices[] = {
      0,  1,  2,  0,  2,  3,  // Front
      5,  4,  7,  5,  7,  6,  // Back
      8,  9,  10, 8,  10, 11, // Left
      14, 13, 12, 14, 12, 15, // Right
      16, 17, 18, 16, 18, 19, // Top
      21, 20, 23, 21, 23, 22, // Bottom
  };

  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f),
  };

  // Generate buffer and array objects
  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  // Bind buffers and upload vertex/index data to the GPU
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Tell the VAO how vertex data is laid out in memory
  // Attribute 0: position (3 floats)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // Attribute 2: texture coordinates (2 floats, offset by 3 floats)
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Unbind VBO and VAO (EBO stays bound to VAO)
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Load and create texture
  Texture containerTexture("assets/textures/container.jpg", GL_RGB, 0,
                           GL_REPEAT, GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR,
                           GL_LINEAR);
  ourShader.use();
  ourShader.setInt("texture1", 0);

  // Creates entities for every cube
  // Sets positions for each entity from cubePositions list
  Registry registry;
  for (glm::vec3 cubePos : cubePositions) {

    // Create entity and adds components
    int entityId = registry.createEntity();
    registry.addComponent<Transform>(entityId);
    registry.addMeshRenderer(entityId, VAO, containerTexture.textureID);

    // Set transform positions from the cube positions list
    Transform *transform = registry.getTransform(entityId);
    if (transform != nullptr) {
      transform->position = cubePos;
      transform->rotation = cubePos;
    }
  }

  // Render loop
  while (!glfwWindowShouldClose(window)) {

    // deltaTime
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Setup
    process_input(window);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Freecam
    ourShader.use();
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    ourShader.setMat4("view", view);
    ourShader.setMat4("projection", projection);

    // Set textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, containerTexture.textureID);

    // Get meshRender from meshRenderers list using id's
    glBindVertexArray(VAO);
    for (auto &mesh : registry.meshRenderers) {

      // Get transform to set positions
      Transform *transform = registry.getTransform(mesh.entityID);
      if (transform != nullptr) {

        // Move to positions
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform->position);

        // Set positions in shader
        ourShader.use();
        ourShader.setMat4("model", model);
      }

      // Draw the cubes
      glBindTexture(GL_TEXTURE_2D, mesh.textureID);
      glBindVertexArray(mesh.VAO);
      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glfwTerminate();
  return 0;
}
