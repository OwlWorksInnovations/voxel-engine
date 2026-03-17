#include "Mesh.hpp"

Mesh::Mesh() : VAO(0), VBO(0), EBO(0), indexCount(0) {}

Mesh::~Mesh() { Clear(); }

void Mesh::Init() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
}

void Mesh::Setup(const std::vector<Vertex> &vertices,
                 const std::vector<unsigned int> &indices) {
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  // Position attribute (location = 0)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, x));
  glEnableVertexAttribArray(0);

  // Texture coord attribute (location = 1)
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, u));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void Mesh::SetData(const std::vector<Vertex> &vertices,
                   const std::vector<unsigned int> &indices) {
  if (!VAO)
    Init();
  indexCount = static_cast<unsigned int>(indices.size());
  Setup(vertices, indices);
}

void Mesh::Draw() {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Mesh::Clear() {
  if (VAO) {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    VAO = VBO = EBO = 0;
    indexCount = 0;
  }
}
