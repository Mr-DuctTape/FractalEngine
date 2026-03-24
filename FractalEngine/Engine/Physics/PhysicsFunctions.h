#pragma once
#include "../EntitySystem/Entities.h"


namespace Physics
{
	namespace Values
	{
		const float worldGravity = 980.0f;
		const float friction = 0.90f;
	}
	void Run(std::vector<Object*>& objects);
}