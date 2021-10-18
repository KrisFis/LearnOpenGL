
#pragma once

#include "ModuleMinimal.h"
#include "SceneObject.h"
#include "Color.h"

// Forward declaration
class FShaderProgram;
class FModel;
class FTexture;

struct FVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class FMesh : public ISceneObject
{

private: // Constructors

	explicit FMesh(const std::vector<FVertex>& InVertices, const std::vector<uint32>& InIndices, const std::vector<TSharedPtr<FTexture>>& InTextures, bool Owned);

public: // Destructors

	virtual ~FMesh();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FMesh> Create(const std::vector<FVertex>& Vertices, const std::vector<uint32>& Indices, const std::vector<TSharedPtr<FTexture>>& Textures, bool Owned = false)
	{ return MakeShareable(new FMesh(Vertices, Indices, Textures, Owned)); }

public: // Getters

	FORCEINLINE bool IsInitialized() const { return bIsInitialized; }
	FORCEINLINE bool IsValid() const { return Vertices.size() > 0; }
	FORCEINLINE bool IsOwned() const { return bIsOwned; };
	FORCEINLINE const std::vector<TSharedPtr<FTexture>>& GetTextures() const { return Textures; }

public: // Setters
	
	void SetTextures(const std::vector<TSharedPtr<FTexture>>& InTextures);
	FORCEINLINE void SetIsOwned(bool Value) { bIsOwned = Value; RecalculateModel(); }

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

	void RecalculateModel();
	void DrawImpl(const TSharedPtr<FShaderProgram>& Shader);

private: // Owner

	TSharedPtr<FModel> Owner;

private: // Fields

	float OutlineSize;
	FColor OutlineColor;

	FTransform Transform;

	std::vector<FVertex> Vertices;
	std::vector<uint32> Indices;
	std::vector<TSharedPtr<FTexture>> Textures;
	
	FVertexArrayId VAO;
	FBufferId VBO, EBO;

private: // Cache

	glm::mat4 CachedModel;

private: // Primitive Fields
	
	uint8 bIsOwned : 1;
	uint8 bCullFaces : 1;
	uint8 bIsInitialized : 1;
};

typedef TSharedPtr<class FMesh> FMeshPtr;