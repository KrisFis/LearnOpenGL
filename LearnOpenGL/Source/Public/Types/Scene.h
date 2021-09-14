
#pragma once

#include "ModuleMinimal.h"

// Forward declaration
class ISceneObject;

class FScene
{
	
public: // Constructors

	FScene(const std::vector<FModelPtr>& InModels, const std::vector<FMeshPtr>& InMeshes);
	virtual ~FScene();
	
public: // Add

	// returns index
	int32 AddModel(FModelPtr Model);
	int32 AddMesh(FMeshPtr Mesh);

public: // Remove

	bool RemoveModelByIdx(uint16 Idx);
	bool RemoveMeshByIdx(uint16 Idx);

public: // Draw

	void Draw(FShaderProgram& Shader);

private: // Fields

	uint16 ModelCounter, MeshCounter;

	std::unordered_map<uint16, FModelPtr> Models;
	std::unordered_map<uint16, FMeshPtr> Meshes;
	
};