
#include "Texture.h"

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <jpgd.h>
#include <lodepng.h>

#include <string>
#include <iostream>

FTexture::FTexture(const char* TextureFilename, const ETextureType TextureType)
	: Id(0)
	, Type(TextureType)
	, bIsInitialized(false)
{
	glGenTextures(1, &Id);
	glBindTexture(GL_TEXTURE_2D, Id);

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	const std::string fullTextPath = std::string("../../Content/Textures/") + std::string(TextureFilename);

	switch (TextureType)
	{
	case ETextureType::JPEG:
		{
			int width, height, nrChannels;
			unsigned char* data = jpgd::decompress_jpeg_image_from_file(fullTextPath.c_str(), &width, &height, &nrChannels, 3);
			if(!data)
			{
				std::cout << "Texture load failed [" << TextureFilename << "] [NOT_FOUND]" << std::endl;
			}
			else if(data[0] == '\0')
			{
				std::cout << "Texture load failed [" << TextureFilename << "] [EMPTY]" << std::endl;
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				jpgd::free_image(data);

				std::cout << "Texture load successful [" << TextureFilename << "]" << std::endl;
			}
		}
		break;
	case ETextureType::PNG:
		{
			std::vector<unsigned char> data;
			unsigned int width, height;
			lodepng::decode(data, width, height, fullTextPath);
			if(data.size() <= 0)
			{
				std::cout << "Texture load failed [" << TextureFilename << "] [NOT_FOUND]" << std::endl;
			}
			else if(data[0] == '\0')
			{
				std::cout << "Texture load failed [" << TextureFilename << "] [EMPTY]" << std::endl;
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
				glGenerateMipmap(GL_TEXTURE_2D);

				std::cout << "Texture load successful [" << TextureFilename << "]" << std::endl;
			}
		}
		break;
	}

	bIsInitialized = true;
}

FTexture::~FTexture()
{}

void FTexture::Apply()
{
	glActiveTexture(GL_TEXTURE0);
 	glBindTexture(GL_TEXTURE_2D, Id);
}
