#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Mesh.hpp"

class InstancedMesh {
public:
  InstancedMesh();
  ~InstancedMesh();

  void SetData(const std::vector<Vertex> &vertices,
               const std::vector<unsigned int> &indices,
               const std::vector<glm::vec3> &instancePositions);

  void DrawInstanced(int instanceCount);
  void Clear();

private:
  unsigned int VAO, VBO, EBO, instanceVBO;
  unsigned int indexCount;
  unsigned int instanceCount;

  void Init();
  void Setup(const std::vector<Vertex> &vertices,
             const std::vector<unsigned int> &indices,
             const std::vector<glm::vec3> &instancePositions);
};