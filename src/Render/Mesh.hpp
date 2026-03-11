#pragma once
#include <glad/glad.h>
#include <vector>

struct Vertex {
  float x, y, z;
  float u, v;
  float brightness;
};

class Mesh {
public:
  Mesh();
  ~Mesh();

  void SetData(const std::vector<Vertex> &vertices,
               const std::vector<unsigned int> &indices);
  void Draw();
  void Clear();

private:
  unsigned int VAO, VBO, EBO;
  unsigned int indexCount;
  bool initialized;

  void Init();
  void Setup(const std::vector<Vertex> &vertices,
             const std::vector<unsigned int> &indices);
};
