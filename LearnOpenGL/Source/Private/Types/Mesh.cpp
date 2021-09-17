
#include "Mesh.h"

#include "Texture.h"
#include "RenderUtils.h"
#include "Shader.h"
#include "ColorUtils.h"

FMesh::FMesh(const std::vector<FVertex>& InVertices, const std::vector<uint32>& InIndices, const std::vector<FTexture>& InTextures, bool Owned) 
	: OutlineSize(0.f)
	, OutlineColor(NColors::Transparent)
	, Transform(FTransform())
	, Vertices(InVertices)
	, Indices(InIndices)
	, Textures(InTextures)
	, bIsOwned(Owned)
	, bCullFaces(true)
	, bIsInitialized(false)
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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)offsetof(FVertex, Normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)offsetof(FVertex, TexCoords));
	glEnableVertexAttribArray(2);
	
	NRenderUtils::UnbindVertexArray(VAO);
	
	RecalculateModel();
	
	bIsInitialized = true;
}

FMesh::~FMesh()
{}

void FMesh::SetTextures(const std::vector<FTexture>& InTextures)
{
	Textures = InTextures;
}

void FMesh::Draw(FShaderProgram& Shader)
{
	if(!IsValid()) return;
	
	const bool shouldOutline = !IsOwned() && IsOutlined(); 
	const bool shouldCull = !IsOwned() && CullsFaces(); 
	
	if(shouldCull)
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
	
	if(shouldOutline)
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	}

	DrawImpl(Shader);
	
	if(shouldOutline)
	{
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x0);
		glDisable(GL_DEPTH_TEST);
	
		Shader.SetBool("useOverrideColor", true);
		Shader.SetVec4("overrideColor", OutlineColor.ToVec4());
		
		Shader.SetMat4("model",
			glm::scale(
				CachedModel,
				glm::vec3(1 + OutlineSize)
			)
		);
		
		NRenderUtils::BindVertexArray(VAO);
		
		glDrawElements(GL_TRIANGLES, (GLsizei)Indices.size(), GL_UNSIGNED_INT, 0);
		
		NRenderUtils::UnbindVertexArray();
		
		Shader.SetBool("useOverrideColor", false);
		
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);
	}
}

void FMesh::RecalculateModel()
{
	if(bIsOwned) return;
	
	CachedModel = Transform.CalculateModelMatrix();
}

void FMesh::DrawImpl(FShaderProgram& Shader)
{
	uint8 diffuseCounter = 0;
	uint8 specularCounter = 0;
	for(uint8 i = 0; i < Textures.size(); ++i)
	{
		std::string nameOfTexture;
		switch (Textures[i].GetType()) 
		{
			case ETextureType::Diffuse:
				nameOfTexture = "diffuse" + std::to_string(diffuseCounter++);
				break;
			case ETextureType::Specular:
				nameOfTexture = "specular" + std::to_string(specularCounter++);
				break;
			default:
				continue;
		}
		
		Shader.SetInt32(("material." + nameOfTexture).c_str(), i);
		Textures[i].Use(i);
	}
	
	if(!IsOwned())
		Shader.SetMat4("model", CachedModel);

	NRenderUtils::BindVertexArray(VAO);
	
	glDrawElements(GL_TRIANGLES, (GLsizei)Indices.size(), GL_UNSIGNED_INT, 0);
	
	NRenderUtils::UnbindVertexArray();
}
