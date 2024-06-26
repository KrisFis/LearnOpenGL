
#include "Mesh.h"

#include "Texture.h"
#include "RenderUtils.h"
#include "ShaderProgram.h"
#include "ColorConsts.h"

FMesh::FMesh(const TArray<FMeshVertex>& InVertices, const TArray<uint32>& InIndices, const TArray<TSharedPtr<FTexture>>& InTextures, bool Owned) 
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
	VAO = NRenderUtils::NVertexArray::Generate();
	
	auto buffers = NRenderUtils::NBuffer::Generate(2);
	VBO = buffers[0];
	EBO = buffers[1];
	
	// Initialize
	NRenderUtils::NVertexArray::Bind(VAO);
	
	// Setup data
	NRenderUtils::NBuffer::Bind(GL_ARRAY_BUFFER, VBO);
	
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(FMeshVertex), &Vertices[0], GL_STATIC_DRAW);
	
	NRenderUtils::NBuffer::Unbind(GL_ARRAY_BUFFER);
	NRenderUtils::NBuffer::Bind(GL_ELEMENT_ARRAY_BUFFER, EBO);
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(uint32), &Indices[0], GL_STATIC_DRAW);
	
	NRenderUtils::NBuffer::Unbind(GL_ELEMENT_ARRAY_BUFFER);
	
	// Attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FMeshVertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(FMeshVertex), (void*)offsetof(FMeshVertex, Normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FMeshVertex), (void*)offsetof(FMeshVertex, TexCoord));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(FMeshVertex), (void*)offsetof(FMeshVertex, Tangent));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(FMeshVertex), (void*)offsetof(FMeshVertex, Bitangent));
	glEnableVertexAttribArray(4);
	
	NRenderUtils::NVertexArray::Unbind();

	RefreshCaches();
	
	bIsInitialized = true;
}

FMesh::~FMesh()
{
	if(!bIsInitialized) return;
	
	NRenderUtils::NVertexArray::Delete(VAO);
	NRenderUtils::NBuffer::Delete({VBO, EBO});
}

void FMesh::SetTextures(const TArray<TSharedPtr<FTexture>>& InTextures)
{
	Textures = InTextures;
}

void FMesh::Draw(const TSharedPtr<FShaderProgram>& Shader)
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
	
		Shader->SetBool("useOverrideColor", true);
		Shader->SetVec4("overrideColor", OutlineColor.ToVec4());
		
		Shader->SetMat4("model",
			glm::scale(
				CachedModel,
				glm::vec3(1 + OutlineSize)
			)
		);
		
		// No lightning required
		Shader->SetMat4("inverseModel", glm::mat3(1.f));
		
		NRenderUtils::NVertexArray::Bind(VAO);
		
		glDrawElements(GL_TRIANGLES, (GLsizei)Indices.size(), GL_UNSIGNED_INT, 0);
		
		NRenderUtils::NVertexArray::Unbind();
		
		Shader->SetBool("useOverrideColor", false);
		
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);
	}
}

void FMesh::RefreshCaches()
{
	if(bIsOwned) return;
	
	CachedModel = Transform.CalculateModelMatrix();
	CachedNormalMatrix = glm::transpose(glm::inverse(CachedModel));
}

void FMesh::DrawImpl(const TSharedPtr<FShaderProgram>& Shader)
{
	uint8 diffuseCounter = 0;
	uint8 specularCounter = 0;
	uint8 normalCounter = 0;
	uint8 heightCounter = 0;
	
	for(uint8 i = 0; i < Textures.size(); ++i)
	{
		FString nameOfTexture;
		switch (Textures[i]->GetType()) 
		{
			case ETextureType::Diffuse:
				nameOfTexture = "diffuse" + std::to_string(diffuseCounter++);
				break;
			case ETextureType::Specular:
				nameOfTexture = "specular" + std::to_string(specularCounter++);
				break;
			case ETextureType::Normals:
				nameOfTexture = "normal" + std::to_string(normalCounter++);
				break;
			case ETextureType::Height:
				nameOfTexture = "height" + std::to_string(heightCounter++);
				break;
			default:
				ENSURE_NO_ENTRY();
				continue;
		}
		
		Shader->SetInt32(("material." + nameOfTexture).c_str(), i);
		Textures[i]->Use(i);
	}
	
	if(!IsOwned())
	{
		Shader->SetMat4("model", CachedModel);
		Shader->SetMat4("inverseModel", CachedNormalMatrix);
	}

	Shader->SetInt32("material.numOfDiffuses", diffuseCounter);
	Shader->SetInt32("material.numOfSpeculars", specularCounter);
	Shader->SetInt32("material.numOfNormals", normalCounter);
	Shader->SetInt32("material.numOfHeights", heightCounter);

	NRenderUtils::NVertexArray::Bind(VAO);
	
	glDrawElements(GL_TRIANGLES, (GLsizei)Indices.size(), GL_UNSIGNED_INT, 0);
	
	NRenderUtils::NVertexArray::Unbind();
	
	for(uint8 i = 0; i < Textures.size(); ++i)
	{
		Textures[i]->Clear();
	}
}
