
#pragma once

#include "ModuleMinimal.h"

// Forward declaration
class ISceneObject;

class FScene
{
	
public: // Constructors

	FScene();
	FScene(const std::vector<FModelPtr>& InModels, const std::vector<FMeshPtr>& InMeshes);
	virtual ~FScene();
	
public: // Count

	inline uint16 GetNumOfModels() const { return (uint16)Models.size(); }
	inline uint16 GetNumOfMeshes() const { return (uint16)Meshes.size(); }
	
public: // Add

	void AddModels(const std::vector<FModelPtr>& InModels);
	void AddMeshes(const std::vector<FMeshPtr>& InMeshes);
	
	// returns index
	int32 AddModel(FModelPtr Model);
	int32 AddMesh(FMeshPtr Mesh);

public: // Get

	FModelPtr GetModelByIdx(uint16 Idx) const;
	FMeshPtr GetMeshByIdx(uint16 Idx) const;

public: // Remove

	bool RemoveModelByIdx(uint16 Idx);
	bool RemoveMeshByIdx(uint16 Idx);

public: // Draw

	void Draw(FShaderProgram& Shader, const FCamera& Camera);

private: // Fields

	uint16 ModelCounter, MeshCounter;

	std::unordered_map<uint16, FModelPtr> Models;
	std::unordered_map<uint16, FMeshPtr> Meshes;
};