
#pragma once

#include "ModuleMinimal.h"
#include "SceneObject.h"
#include "Color.h"

// Forward declaration
class FShaderProgram;
class FModel;
class FTexture;

struct FMeshVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoord;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

class FMesh : public ISceneObject
{

private: // Constructors

	explicit FMesh(const TArray<FMeshVertex>& InVertices, const TArray<uint32>& InIndices, const TArray<TSharedPtr<FTexture>>& InTextures, bool Owned);

public: // Destructors

	virtual ~FMesh();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FMesh> Create(const TArray<FMeshVertex>& Vertices, const TArray<uint32>& Indices, const TArray<TSharedPtr<FTexture>>& Textures, bool Owned = false)
	{ return MakeShareable(new FMesh(Vertices, Indices, Textures, Owned)); }

public: // Getters

	FORCEINLINE bool IsInitialized() const { return bIsInitialized; }
	FORCEINLINE bool IsValid() const { return Vertices.size() > 0; }
	FORCEINLINE bool IsOwned() const { return bIsOwned; };
	FORCEINLINE const TArray<TSharedPtr<FTexture>>& GetTextures() const { return Textures; }

public: // Setters
	
	void SetTextures(const TArray<TSharedPtr<FTexture>>& InTextures);
	FORCEINLINE void SetIsOwned(bool Value) { bIsOwned = Value;
		RefreshCaches(); }

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

	void RefreshCaches();
	void DrawImpl(const TSharedPtr<FShaderProgram>& Shader);

private: // Owner

	TSharedPtr<FModel> Owner;

private: // Fields

	float OutlineSize;
	FColor OutlineColor;

	FTransform Transform;

	TArray<FMeshVertex> Vertices;
	TArray<uint32> Indices;
	TArray<TSharedPtr<FTexture>> Textures;
	
	FVertexArrayId VAO;
	FBufferId VBO, EBO;

private: // Cache

	glm::mat4 CachedModel;
	glm::mat4 CachedNormalMatrix;

private: // Primitive Fields
	
	uint8 bIsOwned : 1;
	uint8 bCullFaces : 1;
	uint8 bIsInitialized : 1;
};

typedef TSharedPtr<class FMesh> FMeshPtr;