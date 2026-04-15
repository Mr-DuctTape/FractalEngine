#pragma once
#include <iostream>

enum class Layer : uint32_t
{
	ERROR = 99999,
	DEFAULT = 0,
	PLAYER,
	ENEMY,
	GROUND,
	TRIGGER
};