
#pragma once

#include "ModuleMinimal.h"

enum class ELightType : uint8
{
	Invalid = 0,
	
	Directional,
	Point,
	Spot
};

class ILight
{

public: // Virtual destructor

	virtual ~ILight() {}

};