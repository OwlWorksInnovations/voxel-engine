#pragma once
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include <cmath>
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
  std::vector<float> sampledValues;
  void generatePerlinNoise() {
    fillCorners();
    for (int x = 0; x < 9; x++) {
      for (int y = 0; y < 9; y++) {
        sampledValues.push_back(sample(x, y));
      }
    }
  }

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

  float sample(float x, float y) {
    // Get cell locations
    // Bottom left
    int blCellX = std::floor(x);
    int blCellY = std::floor(y);
    // Bottom right
    int brCellX = blCellX + 1;
    int brCellY = blCellY;
    // Top left
    int tlCellX = blCellX;
    int tlCellY = blCellY + 1;
    // Top Right
    int trCellX = blCellX + 1;
    int trCellY = blCellY + 1;

    // Calculate dot values
    // Bottom Left
    GridCorner blCorner = grid.corners[blCellX * 10 + blCellY];
    glm::vec2 blToSample = glm::vec2(x, y) - blCorner.point;
    float blDot = glm::dot(blToSample, blCorner.arrowDirection);
    // Bottom Right
    GridCorner brCorner = grid.corners[brCellX * 10 + brCellY];
    glm::vec2 brToSample = glm::vec2(x, y) - brCorner.point;
    float brDot = glm::dot(brToSample, brCorner.arrowDirection);
    // Top Left
    GridCorner tlCorner = grid.corners[tlCellX * 10 + tlCellY];
    glm::vec2 tlToSample = glm::vec2(x, y) - tlCorner.point;
    float tlDot = glm::dot(tlToSample, tlCorner.arrowDirection);
    // Top Right
    GridCorner trCorner = grid.corners[trCellX * 10 + trCellY];
    glm::vec2 trToSample = glm::vec2(x, y) - trCorner.point;
    float trDot = glm::dot(trToSample, trCorner.arrowDirection);

    // Lerp
    float tx = x - blCellX; // e.g. 0.3 if x = 2.3
    float ty = y - blCellY;

    float bottom = lerp(blDot, brDot, fade(tx));
    float top = lerp(tlDot, trDot, fade(tx));
    float result = lerp(bottom, top, fade(ty));

    return result;
  }

  float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
  float lerp(float a, float b, float t) { return a + t * (b - a); }
};
