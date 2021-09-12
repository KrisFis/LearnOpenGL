
#pragma once

#include "ModuleMinimal.h"

struct FVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class FMesh
{

public: // Constructors

	FMesh(const std::vector<FVertex>& InVertices, const std::vector<uint32>& InIndices, const std::vector<FTexture>& InTextures);
	virtual ~FMesh();

public: // Getters

	inline bool IsInitialized() const { return bIsInitialized; }

public: // Setters

	void SetTextures(const std::vector<FTexture>& InTextures);

public: // Methods

	void Draw(FShaderProgram& Shader);

private: // Fields

	std::vector<FVertex> Vertices;
	std::vector<uint32> Indices;
	std::vector<FTexture> Textures;
	
	FVertexArrayId VAO;
	FBufferId VBO, EBO;
	
	bool bIsInitialized;
};