#include "Core/Engine.hpp"
#include <iostream>

int main() {
  Engine::InitWindow(1280, 720, "Voxel Engine");
  Engine::Run();
  Engine::Shutdown();

  return 0;
}
