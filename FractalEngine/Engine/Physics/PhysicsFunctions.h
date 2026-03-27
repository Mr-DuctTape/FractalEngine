#pragma once
#include "../EntitySystem/Entities.h"


namespace Physics
{
	namespace Values
	{
		constexpr float worldGravity = 980.0f;
		constexpr float friction = 0.95f;
	}
	void Run(std::vector<Object*>& objects);
}