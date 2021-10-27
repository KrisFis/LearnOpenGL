
#include "Skybox.h"
#include "Cubemap.h"
#include "RenderUtils.h"
#include "ShaderProgram.h"

namespace NSkybox_Private
{
	constexpr float Vertices[] = {
		-1.f,  1.f, -1.f,
		-1.f, -1.f, -1.f,
		 1.f, -1.f, -1.f,
		 1.f, -1.f, -1.f,
		 1.f,  1.f, -1.f,
		-1.f,  1.f, -1.f,

		-1.f, -1.f,  1.f,
		-1.f, -1.f, -1.f,
		-1.f,  1.f, -1.f,
		-1.f,  1.f, -1.f,
		-1.f,  1.f,  1.f,
		-1.f, -1.f,  1.f,

		 1.f, -1.f, -1.f,
		 1.f, -1.f,  1.f,
		 1.f,  1.f,  1.f,
		 1.f,  1.f,  1.f,
		 1.f,  1.f, -1.f,
		 1.f, -1.f, -1.f,

		-1.f, -1.f,  1.f,
		-1.f,  1.f,  1.f,
		 1.f,  1.f,  1.f,
		 1.f,  1.f,  1.f,
		 1.f, -1.f,  1.f,
		-1.f, -1.f,  1.f,

		-1.f,  1.f, -1.f,
		 1.f,  1.f, -1.f,
		 1.f,  1.f,  1.f,
		 1.f,  1.f,  1.f,
		-1.f,  1.f,  1.f,
		-1.f,  1.f, -1.f,

		-1.f, -1.f, -1.f,
		-1.f, -1.f,  1.f,
		 1.f, -1.f, -1.f,
		 1.f, -1.f, -1.f,
		-1.f, -1.f,  1.f,
		 1.f, -1.f,  1.f
	};
}

FSkybox::FSkybox(const TSharedPtr<FCubemap>& InMap)
	: Map(InMap)
	, VAO(NRenderConsts::Invalid::VertexArrayId)
	, VBO(NRenderConsts::Invalid::BufferId)
	, bIsInitialized(false)
{
	ENSURE_VALID_RET(InMap);
	ENSURE_RET(InMap->IsInitialized());
	
	VAO = NRenderUtils::NVertexArray::Generate();
	VBO = NRenderUtils::NBuffer::Generate();
	
	NRenderUtils::NVertexArray::Bind(VAO);
	NRenderUtils::NBuffer::Bind(GL_ARRAY_BUFFER, VBO);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(NSkybox_Private::Vertices), &NSkybox_Private::Vertices, GL_STATIC_DRAW);
	
	NRenderUtils::NBuffer::Unbind(GL_ARRAY_BUFFER);
	
	// Attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	NRenderUtils::NVertexArray::Unbind();
	
	bIsInitialized = true;
}

FSkybox::~FSkybox()
{
	if(!bIsInitialized) return;

	NRenderUtils::NVertexArray::Delete(VAO);
	NRenderUtils::NBuffer::Delete(VBO);
}

void FSkybox::SetMap(const TSharedPtr<FCubemap>& InMap)
{
	ENSURE_VALID_RET(InMap);
	ENSURE_RET(InMap->IsInitialized());
	
	Map = InMap;
}

void FSkybox::Draw(const TSharedPtr<FShaderProgram>& Shader)
{
	ENSURE_RET(Shader->IsEnabled());
	ENSURE_RET(bIsInitialized);

	glDepthFunc(GL_LEQUAL);
	
	Shader->SetInt32("material.texture", 0);
	Map->Use(0);
	
	NRenderUtils::NVertexArray::Bind(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	NRenderUtils::NVertexArray::Unbind();
	
	Map->Clear();
	
	glDepthFunc(GL_LESS);
}
