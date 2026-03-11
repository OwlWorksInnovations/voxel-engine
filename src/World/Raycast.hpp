#pragma once
#include "World.hpp"
#include <glm/glm.hpp>

struct RaycastResult {
  bool hit;
  glm::ivec3 blockPos; // position of hit block in world voxel coords
  glm::ivec3 normal;   // face normal of hit face
  glm::vec3 hitPoint;  // exact world position of hit
};

inline RaycastResult Raycast(World &world, glm::vec3 origin,
                             glm::vec3 direction, float maxDistance = 10.0f) {
  RaycastResult result;
  result.hit = false;

  direction = glm::normalize(direction);
  const float EPSILON = 1e-6f;
  if (fabs(direction.x) < EPSILON) direction.x = (direction.x >= 0) ? EPSILON : -EPSILON;
  if (fabs(direction.y) < EPSILON) direction.y = (direction.y >= 0) ? EPSILON : -EPSILON;
  if (fabs(direction.z) < EPSILON) direction.z = (direction.z >= 0) ? EPSILON : -EPSILON;

  // convert origin to voxel coords
  glm::ivec3 voxel = glm::ivec3((int)floor(origin.x / VOXEL_SIZE),
                                (int)floor(origin.y / VOXEL_SIZE),
                                (int)floor(origin.z / VOXEL_SIZE));

  // DDA step direction
  glm::ivec3 step =
      glm::ivec3(direction.x > 0 ? 1 : -1, direction.y > 0 ? 1 : -1,
                 direction.z > 0 ? 1 : -1);

  // how far along the ray to cross one voxel in each axis
  glm::vec3 tDelta =
      glm::vec3(fabs(VOXEL_SIZE / direction.x), fabs(VOXEL_SIZE / direction.y),
                fabs(VOXEL_SIZE / direction.z));

  // initial distances to next voxel boundary
  glm::vec3 tMax;
  tMax.x = (step.x > 0)
               ? ((floor(origin.x / VOXEL_SIZE) + 1) * VOXEL_SIZE - origin.x) /
                     direction.x
               : (origin.x - floor(origin.x / VOXEL_SIZE) * VOXEL_SIZE) /
                     -direction.x;
  tMax.y = (step.y > 0)
               ? ((floor(origin.y / VOXEL_SIZE) + 1) * VOXEL_SIZE - origin.y) /
                     direction.y
               : (origin.y - floor(origin.y / VOXEL_SIZE) * VOXEL_SIZE) /
                     -direction.y;
  tMax.z = (step.z > 0)
               ? ((floor(origin.z / VOXEL_SIZE) + 1) * VOXEL_SIZE - origin.z) /
                     direction.z
               : (origin.z - floor(origin.z / VOXEL_SIZE) * VOXEL_SIZE) /
                     -direction.z;

  glm::ivec3 normal = glm::ivec3(0);
  float t = 0.0f;

  while (t < maxDistance) {
    // advance to next voxel
    if (tMax.x < tMax.y && tMax.x < tMax.z) {
      t = tMax.x;
      tMax.x += tDelta.x;
      voxel.x += step.x;
      normal = glm::ivec3(-step.x, 0, 0);
    } else if (tMax.y < tMax.z) {
      t = tMax.y;
      tMax.y += tDelta.y;
      voxel.y += step.y;
      normal = glm::ivec3(0, -step.y, 0);
    } else {
      t = tMax.z;
      tMax.z += tDelta.z;
      voxel.z += step.z;
      normal = glm::ivec3(0, 0, -step.z);
    }

    // check if voxel is solid
    glm::vec3 worldPos = glm::vec3(voxel) * VOXEL_SIZE;
    BlockType block = world.GetBlockVoxel(voxel);

    if (!IsAir(block)) {
      result.hit = true;
      result.blockPos = voxel;
      result.normal = normal;
      result.hitPoint = origin + direction * t;
      return result;
    }
  }

  return result;
}
