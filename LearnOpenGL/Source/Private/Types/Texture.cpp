
#include "Texture.h"
#include "FileUtils.h"

#include <stb_image.h>

#include <string>
#include <iostream>
#include <assert.h>

FTexture::FTexture(const char* TextureFilename, const ETextureType TextureType)
	: Id(0)
	, UseIndex(-1)
	, Type(ETextureType::Invalid)
{
	assert(TextureType != ETextureType::Invalid);

	static bool flipFixed = false;
	if(!flipFixed)
	{
		stbi_set_flip_vertically_on_load(true);
		flipFixed = true;
	}

	glGenTextures(1, &Id);

	const std::string fullTextPath = NFileUtils::ContentPath() + std::string("/Textures/") + std::string(TextureFilename);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(fullTextPath.c_str(), &width, &height, &nrChannels, 0);

	if(!data)
	{
		std::cout << "Texture load failed [" << TextureFilename << "] [NOT_FOUND]" << std::endl;
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
				assert(false);
			return;
		}

		glBindTexture(GL_TEXTURE_2D, Id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

		std::cout << "Texture load successful [" << TextureFilename << "]" << std::endl;
		Type = TextureType;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

FTexture::~FTexture()
{}

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
