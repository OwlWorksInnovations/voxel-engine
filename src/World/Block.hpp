#pragma once
#include <cstdint>

enum class BlockType : uint8_t { Air = 0, Grass, Dirt, Stone, Wood, Leaves };

inline bool IsAir(BlockType type) { return type == BlockType::Air; }
