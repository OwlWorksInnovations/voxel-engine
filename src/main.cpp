#include "Core/Engine.hpp"
#include <iostream>

// Global
const char *title = "Voxel Engine";
int screenWidth = 1280;
int screenHeight = 720;

int main() {
  std::cout << "Hello world!" << std::endl;

  Engine::InitWindow(screenWidth, screenHeight, title);
  Engine::Run();
  Engine::Exit();

  return 0;
}
