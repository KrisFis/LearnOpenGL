
#include "Texture.h"

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <string>
#include <iostream>
#include <assert.h>

FTexture::FTexture(const char* TextureFilename, const ETextureType TextureType)
	: Id(0)
	, UseIndex(-1)
	, Type(TextureType)
	, bIsInitialized(false)
{
	static bool flipFixed = false;
	if(!flipFixed)
	{
		stbi_set_flip_vertically_on_load(true);
		flipFixed = true;
	}

	glGenTextures(1, &Id);
	glBindTexture(GL_TEXTURE_2D, Id);

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	const std::string fullTextPath = std::string("../../Content/Textures/") + std::string(TextureFilename);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(fullTextPath.c_str(), &width, &height, &nrChannels, 0);

	if(!data)
	{
		std::cout << "Texture load failed [" << TextureFilename << "] [NOT_FOUND]" << std::endl;
	}
	else
	{
		switch(TextureType)
		{
			case ETextureType::JPEG:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			break;
			case ETextureType::PNG:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			break;
			default:
				assert(false);
			return;
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);

		std::cout << "Texture load successful [" << TextureFilename << "]" << std::endl;
		bIsInitialized = true;
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
	glActiveTexture(GL_TEXTURE0 + (unsigned char)UseIndex);
	glBindTexture(GL_TEXTURE_2D, 0);

	UseIndex = -1;
}
