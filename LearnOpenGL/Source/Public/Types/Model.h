
#pragma once

#include "ModuleMinimal.h"
#include "SceneObject.h"
#include "Color.h"

// Forward declaration
class FTexture;
class FMesh;

class FModel : public ISceneObject
{

private: // Constructors

	explicit FModel(const char* FilePath);

public: // Destructor

	virtual ~FModel() {}
	
public: // Static constructions

	FORCEINLINE static TSharedPtr<FModel> Create(const char* FilePath)
	{ return MakeShareable(new FModel(FilePath)); }
	
public: // Getters

	FORCEINLINE bool IsInitialized() const { return bIsInitialized; } 
	FORCEINLINE bool IsValid() const { return Meshes.size() > 0; }
	FORCEINLINE const std::string& GetPath() const { return Directory; }
	FORCEINLINE const std::vector<TSharedPtr<FMesh>>& GetMeshes() const { return Meshes; }

public: // ISceneObject overrides
	
	FORCEINLINE virtual bool CullsFaces() const override { return bCullFaces; }
	FORCEINLINE virtual void SetCullFaces(bool Value) override { bCullFaces = Value; }
	
	FORCEINLINE virtual bool IsOutlined() const override { return (OutlineSize > 0.f) && !OutlineColor.IsTransparent(); }
	FORCEINLINE virtual void SetOutlineColor(const FColor& Value) override { OutlineColor = Value; }
	FORCEINLINE virtual void SetOutlineSize(float Value) override { OutlineSize = Value; }
	
	FORCEINLINE virtual FTransform GetTransform() const override { return Transform; }
	FORCEINLINE virtual void SetTransform(const FTransform& Value) override { Transform = Value; RecalculateModel(); }
	
	virtual void Draw(const TSharedPtr<FShaderProgram>& Shader) override;

private: // Helper methods

	void ProcessNode(aiNode* Node, const aiScene* Scene);
	TSharedPtr<FMesh> ProcessMesh(aiMesh* Mesh, const aiScene* Scene);
	std::vector<TSharedPtr<FTexture>> LoadMaterialTextures(aiMaterial* Material, aiTextureType Type);
	void RecalculateModel();

private: // Fields

	float OutlineSize;
	FColor OutlineColor;

	FTransform Transform;

	std::vector<TSharedPtr<FMesh>> Meshes;
	std::string Directory;

private: // Cache

	std::vector<TSharedPtr<FTexture>> LoadedTextures;
	glm::mat4 CachedModel;

private: // Primitive fields
	
	uint8 bCullFaces : 1;
	uint8 bIsInitialized : 1;
};

typedef TSharedPtr<class FModel> FModelPtr;