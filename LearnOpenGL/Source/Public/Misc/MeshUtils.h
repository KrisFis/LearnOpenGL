
#pragma once

#include "ModuleMinimal.h"

namespace NMeshUtils
{
	FSceneObjectPtr ConstructCube(const FTexture Texture);
	FSceneObjectPtr ConstructPlane(const FTexture Texture);
	FSceneObjectPtr ConstructSphere(const FTexture Texture);
	FSceneObjectPtr ConstructCylinder(const FTexture Texture);
}