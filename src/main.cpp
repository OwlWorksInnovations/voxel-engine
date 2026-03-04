#include "Shader.h"
#include "Texture.h"
#include "glm/detail/qualifier.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include <complex>
#include <cstdlib>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window, float &textureAlpha, Shader &ourShader) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    textureAlpha += 0.01f;
    ourShader.setFloat("textureAlpha", textureAlpha);
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    textureAlpha -= 0.01f;
    ourShader.setFloat("textureAlpha", textureAlpha);
  }
}

int main() {
  // Initialize glfw
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create window
  GLFWwindow *window = glfwCreateWindow(800, 600, "Voxel Engine", NULL, NULL);
  if (window == NULL) {
    std::cout << "Window failed to create! :(" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Initialize glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD! :(" << std::endl;
    return -1;
  }

  // Set up viewport and handle resize
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // Set up shaders
  Shader ourShader("shader.vs", "shader.fs");
  float vertices[] = {
      // positions          // tex coords
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
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

  // Set up VBO (Vertex Buffer Object), VAO (Vertex Array Object), EBO (Element
  // Buffer Object)
  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  // Vertices
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Texture coords
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Set up textures
  Texture containerTexture("assets/textures/container.jpg", GL_RGB, 0,
                           GL_REPEAT, GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR,
                           GL_LINEAR);

  Texture awesomefaceTexture("assets/textures/awesomeface.png", GL_RGBA, 0,
                             GL_CLAMP_TO_EDGE, GL_REPEAT,
                             GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);

  ourShader.use();
  ourShader.setInt("texture1", 0);
  ourShader.setInt("texture2", 1);
  float textureAlpha = 0.2f;
  ourShader.setFloat("textureAlpha", textureAlpha);

  glEnable(GL_DEPTH_TEST);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    process_input(window, textureAlpha, ourShader);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ourShader.use();

    glm::mat4 view =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    ourShader.setMat4("view", view);
    ourShader.setMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, containerTexture.textureID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, awesomefaceTexture.textureID);

    glBindVertexArray(VAO);
    for (unsigned int i = 0; i < 10; i++) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * i;
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      ourShader.setMat4("model", model);

      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup and exit
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glfwTerminate();
  return 0;
}
