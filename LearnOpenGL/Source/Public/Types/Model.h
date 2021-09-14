
#pragma once

#include "ModuleMinimal.h"
#include "Mesh.h"
#include "Texture.h"

class FModel : public ISceneObject
{

public: // Constructors

	FModel(const char* FilePath);
	
public: // Getters

	inline bool IsInitialized() const { return bIsInitialized; } 
	inline bool IsValid() const { return Meshes.size() > 0; }
	inline const std::string& GetPath() const { return Directory; }
	inline const std::vector<std::shared_ptr<FMesh>>& GetMeshes() { return Meshes; }

public: // ISceneObject overrides
	
	inline virtual FTransform GetTransform() const override { return Transform; }
	inline virtual void SetTransform(const FTransform& Value) override { Transform = Value; RecalculateModel(); }
	
	virtual void Draw(FShaderProgram& Shader) override;

private: // Helper methods

	void ProcessNode(aiNode* Node, const aiScene* Scene);
	std::shared_ptr<FMesh> ProcessMesh(aiMesh* Mesh, const aiScene* Scene);
	std::vector<FTexture> LoadMaterialTextures(aiMaterial* Material, aiTextureType Type);
	void RecalculateModel();

private: // Fields

	FTransform Transform;

	std::vector<std::shared_ptr<FMesh>> Meshes;
	std::string Directory;

private: // Cache

	std::vector<FTexture> LoadedTextures;
	glm::mat4 CachedModel;

private: // Primitive fields
	
	bool bIsInitialized;
};