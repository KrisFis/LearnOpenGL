
#pragma once

#include "ModuleMinimal.h"

namespace NMeshUtils
{
	FSceneObjectPtr AsScene(const FModelPtr& Model);
	FSceneObjectPtr AsScene(const FMeshPtr& Mesh);

	FMeshPtr ConstructCube(const FTexture Texture);
	FMeshPtr ConstructPlane(const FTexture Texture);
	FMeshPtr ConstructSphere(const FTexture Texture);
	FMeshPtr ConstructCylinder(const FTexture Texture);
}