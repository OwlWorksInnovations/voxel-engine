#pragma once
#include "Chunk.h"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "stb_image_write.h"
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
  float width;
  float height;
  float scale;

  void setMapSize(float mapWidth, float mapHeight, float mapScale) {
    width = mapWidth;
    height = mapHeight;
    scale = mapScale;
  }

  void generatePerlinNoise() {
    fillCorners();
    for (float x = 0; x < width; x += 1.0) {
      for (float y = 0; y < height; y += 1.0) {
        sampledValues.push_back(sample(x / scale, y / scale));
      }
    }
  }

  void generateVoxelData(Chunk &chunk, std::vector<float> &sampledValues,
                         float maxHeight) {
    for (int x = 0; x < 16; x++) {
      for (int z = 0; z < 16; z++) {
        // Get the noise height for this column
        int height = (sampledValues[x * 16 + z] + 1.0f) / 2.0f * maxHeight;

        // Fill voxels from bottom up to height
        for (int y = 0; y < 16; y++) {
          chunk.voxels[x][y][z] = (y <= height) ? 1 : 0;
        }
      }
    }
  }

  void generateImage(const char *filename) {
    std::vector<unsigned char> picture;
    for (float sampledValue : sampledValues) {
      unsigned char pixel = (sampledValue + 1.0f) / 2.0f * 255;
      picture.push_back(pixel);
    }
    stbi_write_png(filename, (int)width, (int)height, 1, picture.data(),
                   (int)width);
  }

private:
  Grid grid;

  void fillCorners() {
    GridCorner corner;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distrib(-1.0f, 1.0f);

    for (float x = 0; x <= width; x++) {
      for (float y = 0; y <= height; y++) {
        corner.point.x = x;
        corner.point.y = y;
        corner.arrowDirection.x = distrib(gen);
        corner.arrowDirection.y = distrib(gen);
        corner.arrowDirection = glm::normalize(corner.arrowDirection);
        grid.corners.push_back(corner);
      }
    }
  }

  float sample(float x, float y) {
    int blCellX = std::floor(x);
    int blCellY = std::floor(y);
    int brCellX = blCellX + 1;
    int brCellY = blCellY;
    int tlCellX = blCellX;
    int tlCellY = blCellY + 1;
    int trCellX = blCellX + 1;
    int trCellY = blCellY + 1;
    int stride = (int)width + 1;

    GridCorner blCorner = grid.corners[blCellX * stride + blCellY];
    glm::vec2 blToSample = glm::vec2(x, y) - blCorner.point;
    float blDot = glm::dot(blToSample, blCorner.arrowDirection);

    GridCorner brCorner = grid.corners[brCellX * stride + brCellY];
    glm::vec2 brToSample = glm::vec2(x, y) - brCorner.point;
    float brDot = glm::dot(brToSample, brCorner.arrowDirection);

    GridCorner tlCorner = grid.corners[tlCellX * stride + tlCellY];
    glm::vec2 tlToSample = glm::vec2(x, y) - tlCorner.point;
    float tlDot = glm::dot(tlToSample, tlCorner.arrowDirection);

    GridCorner trCorner = grid.corners[trCellX * stride + trCellY];
    glm::vec2 trToSample = glm::vec2(x, y) - trCorner.point;
    float trDot = glm::dot(trToSample, trCorner.arrowDirection);

    float tx = x - blCellX;
    float ty = y - blCellY;

    float bottom = lerp(blDot, brDot, fade(tx));
    float top = lerp(tlDot, trDot, fade(tx));
    float result = lerp(bottom, top, fade(ty));

    return result;
  }

  float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
  float lerp(float a, float b, float t) { return a + t * (b - a); }
};
