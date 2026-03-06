#pragma once
#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <random>
#include <vector>

struct GridCorner {
  glm::vec2 point;
  glm::vec2 arrowDirection;
};

struct Grid {
  std::vector<GridCorner> corners;
};

class PerlinNoise {
public:
  void generatePerlinNoise() {}

private:
  Grid grid;

  void fillCorners() {
    GridCorner corner;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(-1, 1);

    for (int x = 0; 10 > x; x++) {
      for (int y = 0; 10 > y; y++) {
        corner.point.x = x;
        corner.point.y = y;
        corner.arrowDirection.x = distrib(gen);
        corner.arrowDirection.y = distrib(gen);
        grid.corners.push_back(corner);
      }
    }
  }

  float lerp(float a, float b, float t) { return a + t * (b - a); }
};
