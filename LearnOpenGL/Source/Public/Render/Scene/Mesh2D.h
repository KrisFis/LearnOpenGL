
#pragma once

#include "ModuleMinimal.h"
#include "SceneObject.h"
#include "Color.h"

// Forward declarations
class FTexture;
class FShaderProgram;

struct FMesh2DVertex
{
	glm::vec2 Position;
	glm::vec2 TexCoord;
};

class FMesh2D : public ISceneObject
{
	
private: // Constructors

	explicit FMesh2D(const TArray<FMesh2DVertex>& InVertices, const TArray<TSharedPtr<FTexture>>& InTextures);

public: // Destructor

	virtual ~FMesh2D();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FMesh2D> Create(const TArray<FMesh2DVertex>& Vertices, const TArray<TSharedPtr<FTexture>>& Textures)
	{ return MakeShareable(new FMesh2D(Vertices, Textures)); }

public: // Getters

	FORCEINLINE bool IsInitialized() const { return bIsInitialized; }
	FORCEINLINE bool IsValid() const { return Vertices.size() > 0; }
	FORCEINLINE const TArray<TSharedPtr<FTexture>>& GetTextures() const { return Textures; }

public: // Setters
	
	void SetTextures(const TArray<TSharedPtr<FTexture>>& InTextures);

public: // ISceneObject overrides

	FORCEINLINE virtual bool CullsFaces() const override { return bCullFaces; }
	FORCEINLINE virtual void SetCullFaces(bool Value) override { bCullFaces = Value; }
	
	FORCEINLINE virtual bool IsOutlined() const override { return (OutlineSize > 0.f) && !OutlineColor.IsTransparent(); }
	FORCEINLINE virtual void SetOutlineColor(const FColor& Value) override { OutlineColor = Value; }
	FORCEINLINE virtual void SetOutlineSize(float Value) override { OutlineSize = Value; }
	
	FORCEINLINE virtual FTransform GetTransform() const override { return Transform; }
	FORCEINLINE virtual void SetTransform(const FTransform& Value) override { Transform = Value; RecalculateModel(); }

	virtual void Draw(const TSharedPtr<FShaderProgram>& Shader) override;

private: // Helper impl methods

	void RecalculateModel();
	void DrawImpl(const TSharedPtr<FShaderProgram>& Shader);

private: // Fields

	float OutlineSize;
	FColor OutlineColor;

	FTransform Transform;

	TArray<FMesh2DVertex> Vertices;
	TArray<TSharedPtr<FTexture>> Textures;
	
	FVertexArrayId VAO;
	FBufferId VBO;

private: // Cache

	glm::mat4 CachedModel;

private: // Primitive Fields
	
	uint8 bCullFaces : 1;
	uint8 bIsInitialized : 1;
	
};

typedef TSharedPtr<FMesh2D> FMesh2DPtr;