#include "Core/Engine.hpp"
#include <iostream>

int main() {
  try {
    Engine::InitWindow(1280, 720, "Voxel Engine");
    Engine::Run();
    Engine::Shutdown();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  return 0;
}
