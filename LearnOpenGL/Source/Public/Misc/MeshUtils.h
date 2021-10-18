
#pragma once

#include "ModuleMinimal.h"
#include "SceneObject.h"

// Forward declaration
class FTexture;

namespace NMeshUtils
{
	FSceneObjectPtr ConstructCube(const TSharedPtr<FTexture> Texture);
	FSceneObjectPtr ConstructPlane(const TSharedPtr<FTexture> Texture);
	FSceneObjectPtr ConstructSphere(const TSharedPtr<FTexture> Texture);
	FSceneObjectPtr ConstructCylinder(const TSharedPtr<FTexture> Texture);
}