
#pragma once

#include "ModuleMinimal.h"
#include "Mesh.h"
#include "Texture.h"
#include "Color.h"

class FModel : public ISceneObject
{

public: // Constructors

	FModel(const char* FilePath);
	
public: // Getters

	inline bool IsInitialized() const { return bIsInitialized; } 
	inline bool IsValid() const { return Meshes.size() > 0; }
	inline const std::string& GetPath() const { return Directory; }
	inline const std::vector<TSharedPtr<FMesh>>& GetMeshes() const { return Meshes; }

public: // ISceneObject overrides
	
	inline virtual bool CullsFaces() const override { return bCullFaces; }
	inline virtual void SetCullFaces(bool Value) override { bCullFaces = Value; }
	
	inline virtual bool IsOutlined() const override { return (OutlineSize > 0.f) && !OutlineColor.IsTransparent(); }
	inline virtual void SetOutlineColor(const FColor& Value) override { OutlineColor = Value; }
	inline virtual void SetOutlineSize(float Value) override { OutlineSize = Value; }
	
	inline virtual FTransform GetTransform() const override { return Transform; }
	inline virtual void SetTransform(const FTransform& Value) override { Transform = Value; RecalculateModel(); }
	
	virtual void Draw(FShaderProgram& Shader) override;

private: // Helper methods

	void DrawImpl(FShaderProgram& Shader);

	void ProcessNode(aiNode* Node, const aiScene* Scene);
	TSharedPtr<FMesh> ProcessMesh(aiMesh* Mesh, const aiScene* Scene);
	std::vector<FTexture> LoadMaterialTextures(aiMaterial* Material, aiTextureType Type);
	void RecalculateModel();

private: // Fields

	float OutlineSize;
	FColor OutlineColor;

	FTransform Transform;

	std::vector<TSharedPtr<FMesh>> Meshes;
	std::string Directory;

private: // Cache

	std::vector<FTexture> LoadedTextures;
	glm::mat4 CachedModel;

private: // Primitive fields
	
	uint8 bCullFaces : 1;
	uint8 bIsInitialized : 1;
};