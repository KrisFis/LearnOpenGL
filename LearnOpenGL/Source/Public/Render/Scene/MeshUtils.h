
#pragma once

#include "ModuleMinimal.h"
#include "SceneObject.h"

// Forward declaration
class FTexture;

namespace NMeshUtils
{
	FSceneObjectPtr ConstructCube(const TArray<TSharedPtr<FTexture>> Textures);
	FSceneObjectPtr ConstructPlane(const TArray<TSharedPtr<FTexture>> Textures);
	FSceneObjectPtr ConstructSphere(const TArray<TSharedPtr<FTexture>> Textures);
	FSceneObjectPtr ConstructCylinder(const TArray<TSharedPtr<FTexture>> Textures);
}