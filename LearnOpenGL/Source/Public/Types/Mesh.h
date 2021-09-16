
#pragma once

#include "ModuleMinimal.h"
#include "SceneObject.h"
#include "Color.h"

struct FVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class FMesh : public ISceneObject
{

public: // Constructors

	FMesh(const std::vector<FVertex>& InVertices, const std::vector<uint32>& InIndices, const std::vector<FTexture>& InTextures, bool Owned = false);
	virtual ~FMesh();

public: // Getters

	inline bool IsInitialized() const { return bIsInitialized; }
	inline bool IsValid() const { return Vertices.size() > 0; }
	inline bool IsOwned() const { return bIsOwned; };

public: // Setters
	
	void SetTextures(const std::vector<FTexture>& InTextures);
	inline void SetIsOwned(bool Value) { bIsOwned = Value; RecalculateModel(); }

public: // ISceneObject overrides

	inline virtual bool IsOutlined() const override { return (OutlineSize > 0.f) && !OutlineColor.IsTransparent(); }
	inline virtual void SetOutlineColor(const FColor& Value) override { OutlineColor = Value; }
	inline virtual void SetOutlineSize(float Value) override { OutlineSize = Value; }
	
	inline virtual FTransform GetTransform() const override { return Transform; }
	inline virtual void SetTransform(const FTransform& Value) override { Transform = Value; RecalculateModel(); }
	
	virtual void Draw(FShaderProgram& Shader) override;

private: // Helper methods

	void RecalculateModel();
	void DrawImpl(FShaderProgram& Shader);

private: // Owner

	FModelPtr Owner;

private: // Fields

	float OutlineSize;
	FColor OutlineColor;

	FTransform Transform;

	std::vector<FVertex> Vertices;
	std::vector<uint32> Indices;
	std::vector<FTexture> Textures;
	
	FVertexArrayId VAO;
	FBufferId VBO, EBO;

private: // Cache

	glm::mat4 CachedModel;

private: // Primitive Fields
	
	uint8 bIsOwned : 1;
	uint8 bIsInitialized : 1;
};