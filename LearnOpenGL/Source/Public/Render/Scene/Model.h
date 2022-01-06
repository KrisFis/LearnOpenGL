
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
	FORCEINLINE const FString& GetPath() const { return Directory; }
	FORCEINLINE const TArray<TSharedPtr<FMesh>>& GetMeshes() const { return Meshes; }

public: // ISceneObject overrides
	
	FORCEINLINE virtual bool CullsFaces() const override { return bCullFaces; }
	FORCEINLINE virtual void SetCullFaces(bool Value) override { bCullFaces = Value; }
	
	FORCEINLINE virtual bool IsOutlined() const override { return (OutlineSize > 0.f) && !OutlineColor.IsTransparent(); }
	FORCEINLINE virtual void SetOutlineColor(const FColor& Value) override { OutlineColor = Value; }
	FORCEINLINE virtual void SetOutlineSize(float Value) override { OutlineSize = Value; }
	
	FORCEINLINE virtual FTransform GetTransform() const override { return Transform; }
	FORCEINLINE virtual void SetTransform(const FTransform& Value) override { Transform = Value;
		RefreshCaches(); }
	
	virtual void Draw(const TSharedPtr<FShaderProgram>& Shader) override;

private: // Helper methods

	void ProcessNode(aiNode* Node, const aiScene* Scene);
	TSharedPtr<FMesh> ProcessMesh(aiMesh* Mesh, const aiScene* Scene);
	TArray<TSharedPtr<FTexture>> LoadMaterialTextures(aiMaterial* Material, aiTextureType Type);
	void RefreshCaches();

private: // Fields

	float OutlineSize;
	FColor OutlineColor;

	FTransform Transform;

	TArray<TSharedPtr<FMesh>> Meshes;
	FString Directory;

private: // Cache

	TArray<TSharedPtr<FTexture>> LoadedTextures;
	glm::mat4 CachedModel;
	glm::mat3 CachedNormalMatrix;

private: // Primitive fields
	
	uint8 bCullFaces : 1;
	uint8 bIsInitialized : 1;
};

typedef TSharedPtr<class FModel> FModelPtr;