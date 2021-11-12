
#include "Cubemap.h"
#include "stb_image.h"

FCubemap::FCubemap(const TFastMap<ECubemapFace, FString>& FacesPaths)
	: Id(NRenderConsts::Invalid::TextureId)
	, UsedIndex(-1)
	, bIsUsed(false)
	, bIsInitialized(false)
{
	glGenTextures(1, &Id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, Id);

	stbi_set_flip_vertically_on_load(false);

	int32 width, height, nrChannels;
	for(const auto& facePath : FacesPaths)
	{
		GLenum faceAsGLEnum;
		switch (facePath.first)
		{
			case ECubemapFace::Right:
				faceAsGLEnum = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			break;
			case ECubemapFace::Left:
				faceAsGLEnum = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
			break;
			case ECubemapFace::Top:
				faceAsGLEnum = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
			break;
			case ECubemapFace::Bottom:
				faceAsGLEnum = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
			break;
			case ECubemapFace::Front:
				faceAsGLEnum = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
			break;
			case ECubemapFace::Back:
				faceAsGLEnum = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
			break;
			default:
				ENSURE_NO_ENTRY();
			return;
		}
		
		unsigned char* data = stbi_load(facePath.second.c_str(), &width, &height, &nrChannels, 0);
		if (data)
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
		
			glTexImage2D(faceAsGLEnum, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
			
			std::cout << "Cubemap face load successful [Path: " << facePath.second << " ]" << std::endl;
		}
		else
		{
			std::cout << "Cubemap face load failed [Path: " << facePath.second << " ]" << std::endl;
			stbi_image_free(data);
			
			return;
		}
	}
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	bIsInitialized = true;
}

FCubemap::~FCubemap()
{
	Clear();

	if(Id != NRenderConsts::Invalid::TextureId)
	{
		glDeleteTextures(1, &Id);
	}
}

void FCubemap::Use(const uint8 UseIndex)
{
	if(bIsUsed) return;
	
	glActiveTexture(GL_TEXTURE0 + UseIndex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, Id);
	
	bIsUsed = true;
}

void FCubemap::Clear()
{
	if(!bIsUsed) return;
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, NRenderConsts::Invalid::TextureId);
	
	bIsUsed = false;
}
