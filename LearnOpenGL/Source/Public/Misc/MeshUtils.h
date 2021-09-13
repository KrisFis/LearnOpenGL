
#pragma once

#include "ModuleMinimal.h"

// forward declaration
class FMesh;

namespace NMeshUtils
{
	typedef std::shared_ptr<FMesh> FMeshPtr;

	FMeshPtr ConstructCube(const std::vector<FTexture>& Textures = {});
	FMeshPtr ConstructPlane(const std::vector<FTexture>& Textures = {});
	FMeshPtr ConstructSphere(const std::vector<FTexture>& Textures = {});
	FMeshPtr ConstructCylinder(const std::vector<FTexture>& Textures = {});
}