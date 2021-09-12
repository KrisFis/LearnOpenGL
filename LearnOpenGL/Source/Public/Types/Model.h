
#pragma once

#include "ModuleMinimal.h"

class FModel 
{

public: // Constructors

	FModel(const char* FilePath);
	
public: // Getters

	inline bool IsInitialized() const { return bIsInitialized; } 

public: // Draw methods
	
	void Draw(FShaderProgram& Shader);
	
private: // Helper methods

	void ProcessNode(aiNode* Node, const aiScene* Scene);
	FMesh ProcessMesh(aiMesh* Mesh, const aiScene* Scene);
	std::vector<FTexture> LoadMaterialTextures(aiMaterial* Material, aiTextureType Type);

private:

	std::vector<FMesh> Meshes;
	std::string Directory;
	
	bool bIsInitialized;
};