#include "FractalEngine.h"
#include <iostream>

bool FractalEngine::running = true;

void FractalEngine::PrintEngineStatistics()
{
	Scene* scene = SceneManager::GetCurrentScene();
	if (!scene) return;

	unsigned int objectCount = scene->objects.size();
	unsigned int memoryUsageBytes = 0;
	unsigned int collisionChecks = FractalEngineCore::collisionChecks;
	for (auto& ptr : scene->objects)
	{
		memoryUsageBytes += ptr->GetMemoryUsage();
	}
	std::cout
		<< "==================== Scene Stats ====================\n"
		<< "Objects in Scene   : " << objectCount << "\n"
		<< "Memory Usage       : " << memoryUsageBytes << " bytes\n"
		<< "Collision Checks   : " << collisionChecks << "\n"
		<< "=====================================================\n";
}