
#pragma once

#include "ModuleMinimal.h"
#include "Mesh.h"
#include "Texture.h"

class FModel 
{

public: // Constructors

	FModel(const char* FilePath);
	
public: // Getters

	inline bool IsInitialized() const { return bIsInitialized; } 
	inline const std::string& GetPath() const { return Directory; }

public: // Draw methods
	
	void Draw(FShaderProgram& Shader);
	
private: // Helper methods

	void ProcessNode(aiNode* Node, const aiScene* Scene);
	FMesh ProcessMesh(aiMesh* Mesh, const aiScene* Scene);
	std::vector<FTexture> LoadMaterialTextures(aiMaterial* Material, aiTextureType Type);

private: // Fields

	std::vector<FMesh> Meshes;
	std::string Directory;

private: // Opt Cache

	std::vector<FTexture> LoadedTextures;

private: // Primitive fields
	
	bool bIsInitialized;
};