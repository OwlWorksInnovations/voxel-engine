#include "InstancedMesh.hpp"

InstancedMesh::InstancedMesh()
    : VAO(0), VBO(0), EBO(0), instanceVBO(0), indexCount(0), instanceCount(0) {}

InstancedMesh::~InstancedMesh() { Clear(); }

void InstancedMesh::Init() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glGenBuffers(1, &instanceVBO);
}

void InstancedMesh::Setup(const std::vector<Vertex> &vertices,
                          const std::vector<unsigned int> &indices,
                          const std::vector<glm::vec3> &instancePositions) {
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
              vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
              indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, x));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, u));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferData(GL_ARRAY_BUFFER, instancePositions.size() * sizeof(glm::vec3),
               instancePositions.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
  glEnableVertexAttribArray(2);
  glVertexAttribDivisor(2, 1);

  glBindVertexArray(0);
}

void InstancedMesh::SetData(const std::vector<Vertex> &vertices,
                            const std::vector<unsigned int> &indices,
                            const std::vector<glm::vec3> &instancePositions) {
  if (!VAO)
    Init();
  indexCount = static_cast<unsigned int>(indices.size());
  instanceCount = static_cast<unsigned int>(instancePositions.size());
  Setup(vertices, indices, instancePositions);
}

void InstancedMesh::DrawInstanced(int count) {
  glBindVertexArray(VAO);
  glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, count);
  glBindVertexArray(0);
}

void InstancedMesh::Clear() {
  if (VAO) {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &instanceVBO);
    VAO = VBO = EBO = instanceVBO = 0;
    indexCount = 0;
    instanceCount = 0;
  }
}