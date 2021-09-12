
#include "Mesh.h"

#include "Texture.h"
#include "RenderUtils.h"
#include "Shader.h"

FMesh::FMesh(const std::vector<FVertex> &InVertices, const std::vector<uint32> &InIndices, const std::vector<FTexture> &InTextures) 
	: Vertices(InVertices)
	, Indices(InIndices)
	, Textures(InTextures)
{
	// Generate buffers
	VAO = NRenderUtils::GenerateVertexArray();
	
	auto buffers = NRenderUtils::GenerateBuffers(2);
	VBO = buffers[0];
	EBO = buffers[1];
	
	// Initialize
	NRenderUtils::BindVertexArray(VAO);
	
	// Setup data
	NRenderUtils::BindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(FVertex), &Vertices[0], GL_STATIC_DRAW);
	
	NRenderUtils::UnbindBuffer(GL_ARRAY_BUFFER);
	NRenderUtils::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(uint32), &Indices[0], GL_STATIC_DRAW);
	
	NRenderUtils::UnbindBuffer(GL_ELEMENT_ARRAY_BUFFER);
	
	// Attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)offsetof(FVertex, Normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)offsetof(FVertex, TexCoords));
	glEnableVertexAttribArray(2);
	
	NRenderUtils::UnbindVertexArray(VAO);
}

FMesh::~FMesh()
{}

void FMesh::Draw(FShaderProgram& Shader)
{
//	for(uint16 i = 0; i < Textures.size(); ++i)
//	{
//		Textures[i].Use(i);
//
//		switch (Textures[i].GetType()) 
//		{
//		case ETextureType::Diffuse:
//		Shader.SetInt32("texture_diffuse", i);
//		break;
//		case ETextureType::Specular:
//		Shader.SetInt32("texture_specular", i);
//		break;
//		default:
//			assert(false);
//		break;
//		}
//	}
}
