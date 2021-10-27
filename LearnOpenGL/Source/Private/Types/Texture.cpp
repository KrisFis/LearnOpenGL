
#include "Texture.h"
#include "FileUtils.h"
#include "RenderTexture.h"

#include <stb_image.h>
#include <iostream>

ETextureType ToTextureType(const aiTextureType Type) 
{
	switch(Type)
	{
		case aiTextureType_DIFFUSE:
			return ETextureType::Diffuse;
		case aiTextureType_SPECULAR:
			return ETextureType::Specular;
		case aiTextureType_AMBIENT:
			return ETextureType::Ambient;
		case aiTextureType_EMISSIVE:
			return ETextureType::Emissive;
		case aiTextureType_HEIGHT:
			return ETextureType::Height;
		case aiTextureType_NORMALS:
			return ETextureType::Normals;
		case aiTextureType_SHININESS:
			return ETextureType::Shininess;
		case aiTextureType_OPACITY:
			return ETextureType::Opacity;
		case aiTextureType_DISPLACEMENT:
			return ETextureType::Displacement;
		case aiTextureType_LIGHTMAP:
			return ETextureType::LightMap;
		case aiTextureType_REFLECTION:
			return ETextureType::Reflection;
		case aiTextureType_BASE_COLOR:
			return ETextureType::BaseColor;
		case aiTextureType_NORMAL_CAMERA:
			return ETextureType::NormalCamera;
		case aiTextureType_EMISSION_COLOR:
			return ETextureType::EmissionColor;
		case aiTextureType_METALNESS:
			return ETextureType::Metalness;
		case aiTextureType_DIFFUSE_ROUGHNESS:
			return ETextureType::DiffuseRoughness;
		case aiTextureType_AMBIENT_OCCLUSION:
			return ETextureType::AmbientOcclusion;
		default:
			ENSURE_NO_ENTRY();
			return ETextureType::Invalid;
	}
}

FTexture::FTexture(const FRenderTexture* RenderTexture, const ETextureType InType)
	: Id(RenderTexture->GetId())
	, UseIndex(-1)
	, Type(InType)
{}

FTexture::FTexture(const char* InFilePath, const ETextureType InType, bool ClampToEdge)
	: Id(0)
	, UseIndex(-1)
	, Type(ETextureType::Invalid)
{
	ENSURE(InType != ETextureType::Invalid);
	
	stbi_set_flip_vertically_on_load(true);

	glGenTextures(1, &Id);

	//const FString fullTextPath = NFileUtils::ContentPath() + FString("/Textures/") + FString(TextureFilename);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(InFilePath, &width, &height, &nrChannels, 0);

	if(!data)
	{
		std::cout << "Texture load failed [" << InFilePath << "] [NOT_FOUND]" << std::endl;
	}
	else
	{
		GLenum format;
		switch(nrChannels)
		{
			case 1:
				format = GL_RED;
			break;
			case 3:
				format = GL_RGB;
			break;
			case 4:
				format = GL_RGBA;
			break;
			default:
				ENSURE_NO_ENTRY();
			return;
		}

		glBindTexture(GL_TEXTURE_2D, Id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (ClampToEdge) ? GL_CLAMP_TO_EDGE : GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (ClampToEdge) ? GL_CLAMP_TO_EDGE : GL_REPEAT);

		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

		std::cout << "Texture load successful [" << InFilePath << "]" << std::endl;
		FilePath = InFilePath;
		Type = InType;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

FTexture::~FTexture()
{
	if(Id != NRenderConsts::Invalid::TextureId)
		glDeleteTextures(1, &Id);
}

void FTexture::Use(const uint8 Index)
{
	glActiveTexture(GL_TEXTURE0 + Index);
 	glBindTexture(GL_TEXTURE_2D, Id);

	UseIndex = Index;
}

void FTexture::Clear()
{
	if(UseIndex == -1) return;

	glActiveTexture(GL_TEXTURE0 + (unsigned char)UseIndex);
	glBindTexture(GL_TEXTURE_2D, 0);

	UseIndex = -1;
}