#include "Mesh.hpp"

Mesh::Mesh() : VAO(0), VBO(0), EBO(0), indexCount(0), initialized(false) {}

void Mesh::Init() {
  if (initialized)
    return;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  initialized = true;
}

Mesh::~Mesh() { Clear(); }

void Mesh::SetData(const std::vector<Vertex> &vertices,
                   const std::vector<unsigned int> &indices) {
  Init();
  indexCount = indices.size();
  Setup(vertices, indices);
}

void Mesh::Setup(const std::vector<Vertex> &vertices,
                 const std::vector<unsigned int> &indices) {
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_DYNAMIC_DRAW);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, x));
  glEnableVertexAttribArray(0);

  // texture coords
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, u));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void Mesh::Draw() {
  if (!initialized || indexCount == 0)
    return;
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Mesh::Clear() {
  if (!initialized)
    return;
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  VAO = VBO = EBO = 0;
  indexCount = 0;
  initialized = false;
}
