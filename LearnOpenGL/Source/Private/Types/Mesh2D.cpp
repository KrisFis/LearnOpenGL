
#include "Mesh2D.h"
#include "RenderUtils.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "ColorUtils.h"

FMesh2D::FMesh2D(const TArray <FMesh2DVertex>& InVertices, const TArray<TSharedPtr<FTexture>>& InTextures)
	: OutlineSize(0.f)
	, OutlineColor(NColors::Transparent)
	, Transform(FTransform())
	, Vertices(InVertices)
	, Textures(InTextures)
	, bCullFaces(true)
	, bIsInitialized(false)
{
	// Generate buffers
	VAO = NRenderUtils::GenerateVertexArray();
	VBO = NRenderUtils::GenerateBuffer();
	
	// Initialize
	NRenderUtils::BindVertexArray(VAO);
	
	// Setup data
	NRenderUtils::BindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(FMesh2DVertex), &Vertices[0], GL_STATIC_DRAW);
	
	NRenderUtils::UnbindBuffer(GL_ARRAY_BUFFER);
	
	// Attributes
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(FMesh2DVertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FMesh2DVertex), (void*)offsetof(FMesh2DVertex, TexCoord));
	glEnableVertexAttribArray(1);
	
	NRenderUtils::UnbindVertexArray(VAO);
	
	RecalculateModel();
	
	bIsInitialized = true;
}

FMesh2D::~FMesh2D()
{}

void FMesh2D::SetTextures(const TArray<TSharedPtr<FTexture>>& InTextures)
{
	Textures = InTextures;
}

void FMesh2D::Draw(const TSharedPtr<FShaderProgram>& Shader)
{
	if(!IsValid()) return;
		
	const bool shouldOutline = IsOutlined(); 
	const bool shouldCull = CullsFaces(); 
	
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
		
		NRenderUtils::BindVertexArray(VAO);
		
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)Vertices.size());
		
		NRenderUtils::UnbindVertexArray();
		
		Shader->SetBool("useOverrideColor", false);
		
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);
	}
}

void FMesh2D::RecalculateModel()
{
	CachedModel = Transform.CalculateModelMatrix();
}

void FMesh2D::DrawImpl(const TSharedPtr<FShaderProgram>& Shader)
{
	uint8 diffuseCounter = 0;
	uint8 specularCounter = 0;
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
			default:
				continue;
		}
		
		Shader->SetInt32(("material." + nameOfTexture).c_str(), i);
		Textures[i]->Use(i);
	}
	
	//Shader->SetMat4("model", CachedModel);

	NRenderUtils::BindVertexArray(VAO);
	
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)Vertices.size());
	
	NRenderUtils::UnbindVertexArray();
}
