#include "Entities.h"
#include "../Core/FractalEngineCore.h"

unsigned int GameObject::IDNumber = 0;

Camera camera;
unsigned int& Camera::screenWidth = FractalEngineCore::width;
unsigned int& Camera::screenHeight = FractalEngineCore::height;