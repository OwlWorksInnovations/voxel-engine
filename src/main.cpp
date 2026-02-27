#include "Shader.h"
#include "Texture.h"
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glad/glad.h>
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

  // Triangle
  float vertices[] = {
      // positions          // colors           // texture coords
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
  };

  unsigned int indices[] = {
      0, 1, 2, // 1
      0, 2, 3, // 2
  };

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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // Color
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // Texture coords
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
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

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    // Input
    process_input(window, textureAlpha, ourShader);
    // Rendering starts here
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw rectangle
    ourShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, containerTexture.textureID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, awesomefaceTexture.textureID);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Rendering ends here
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup and exit
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glfwTerminate();
  return 0;
}
