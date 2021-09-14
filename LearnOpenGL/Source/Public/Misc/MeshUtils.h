
#pragma once

#include "ModuleMinimal.h"

namespace NMeshUtils
{
	typedef std::shared_ptr<FMesh> FMeshPtr;

	FMeshPtr ConstructCube(const FTexture Texture);
	FMeshPtr ConstructPlane(const FTexture Texture);
	FMeshPtr ConstructSphere(const FTexture Texture);
	FMeshPtr ConstructCylinder(const FTexture Texture);
}