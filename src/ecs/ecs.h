#pragma once
#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <vector>

// Components (move to components.h in the future)
struct Transform {
  int entityID;
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
};

struct MeshRenderer {
  int entityID;
  unsigned int VAO;
  unsigned int textureID;
};

// Registry Class
class Registry {
public:
  int nextID = 0;
  std::vector<Transform> transforms;
  std::vector<MeshRenderer> meshRenderers;

  int createEntity() {
    nextID++;
    return nextID;
  }
  template <typename T> void addComponent(int ID) {}

  // Add
  void addMeshRenderer(int id, unsigned int VAO, unsigned int textureID) {
    MeshRenderer meshRenderer;
    meshRenderer.entityID = id;
    meshRenderer.VAO = VAO;
    meshRenderer.textureID = textureID;
    meshRenderers.push_back(meshRenderer);
  }

  // Get
  Transform *getTransform(int entityID) {
    for (Transform &transform : transforms) {
      if (transform.entityID == entityID) {
        return &transform;
      }
    }
    return nullptr;
  }
};

template <> void Registry::addComponent<Transform>(int id) {
  Transform transform;
  transform.entityID = id;
  transform.position = glm::vec3(0, 0, 0);
  transform.rotation = glm::vec3(0, 0, 0);
  transform.scale = glm::vec3(1, 1, 1);
  transforms.push_back(transform);
};
