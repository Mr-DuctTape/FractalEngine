#include "Entities.h"
#include "../FractalEngineCore.h"

unsigned int GameObject::IDNumber = 0;

Camera camera;
std::vector<std::unique_ptr<GameObject>> objects = {};

unsigned int* screenWidth = &FractalEngineCore::width;
unsigned int* screenHeight = &FractalEngineCore::height;