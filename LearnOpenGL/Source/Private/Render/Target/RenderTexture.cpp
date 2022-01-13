
#include "RenderTexture.h"
#include "Framebuffer.h"

FRenderTexture::FRenderTexture(uint8 InSamples, uint16 InWidth, uint16 InHeight, ERenderTargetAttachmentType InType, uint32 InFlags)
	: Id(0)
	, Type(ERenderTargetAttachmentType::Invalid)
	, Width(0)
	, Height(0)
	, Samples(0)
	, bIsAttached(false)
{
	GLenum internalFormat, format, type;
	switch (InType)
	{
		case ERenderTargetAttachmentType::Color:
			
			if(InFlags & ERenderTextureFlag::UByte)
			{
				type = GL_UNSIGNED_BYTE;
			}
			else if(InFlags & ERenderTextureFlag::Float)
			{
				type = GL_FLOAT;
			}
			else
			{
				ENSURE_NO_ENTRY_RET();
			}

			if(InFlags & ERenderTextureFlag::R)
			{
				format = GL_RED;
			}
			else if(InFlags & ERenderTextureFlag::RG)
			{
				format = GL_RG;
			}
			else if(InFlags & ERenderTextureFlag::RGB)
			{
				format = GL_RGB;
			}
			else if(InFlags & ERenderTextureFlag::RGBA)
			{
				format = GL_RGBA;
			}
			else
			{
				ENSURE_NO_ENTRY_RET();
			}

			if(InFlags & ERenderTextureFlag::Size8)
			{
				if(type == GL_UNSIGNED_BYTE || type == GL_FLOAT)
				{
					if(format == GL_RED)
					{
						internalFormat = GL_R8;
					}
					else if(format == GL_RG)
					{
						internalFormat = GL_RG8;
					}
					else if(format == GL_RGB)
					{
						internalFormat = GL_RGB8;
					}
					else if(format == GL_RGBA)
					{
						internalFormat = GL_RGBA8;
					}
				}
				else
				{
					ENSURE_NO_ENTRY_RET();
				}
			}
			else if(InFlags & ERenderTextureFlag::Size16)
			{
				if(type == GL_UNSIGNED_BYTE)
				{
					if(format == GL_RED)
					{
						internalFormat = GL_R16;
					}
					else if(format == GL_RG)
					{
						internalFormat = GL_RG16;
					}
					else if(format == GL_RGB)
					{
						internalFormat = GL_RGB16;
					}
					else if(format == GL_RGBA)
					{
						internalFormat = GL_RGBA16;
					}
				}
				else if(type == GL_FLOAT)
				{
					if(format == GL_RED)
					{
						internalFormat = GL_R16F;
					}
					else if(format == GL_RG)
					{
						internalFormat = GL_RG16F;
					}
					else if(format == GL_RGB)
					{
						internalFormat = GL_RGB16F;
					}
					else if(format == GL_RGBA)
					{
						internalFormat = GL_RGBA16F;
					}
				}
				else
				{
					ENSURE_NO_ENTRY_RET();
				}
			}
			else if(InFlags & ERenderTextureFlag::Size32)
			{
				if(type == GL_UNSIGNED_BYTE)
				{
					// Unsupported
					ENSURE_NO_ENTRY_RET();
				}
				else if(type == GL_FLOAT)
				{
					if(format == GL_RED)
					{
						internalFormat = GL_R32F;
					}
					else if(format == GL_RG)
					{
						internalFormat = GL_RG32F;
					}
					else if(format == GL_RGB)
					{
						internalFormat = GL_RGB32F;
					}
					else if(format == GL_RGBA)
					{
						internalFormat = GL_RGBA32F;
					}
				}
				else
				{
					ENSURE_NO_ENTRY_RET();
				}
			}
			else
			{
				ENSURE_NO_ENTRY_RET();
			}
			
			break;
		case ERenderTargetAttachmentType::DepthOnly:
			internalFormat = format = GL_DEPTH_COMPONENT;
			type = GL_UNSIGNED_INT;
			break;
		case ERenderTargetAttachmentType::DepthAndStencil:
			internalFormat = GL_DEPTH_STENCIL;
			format = GL_DEPTH24_STENCIL8;
			type = GL_UNSIGNED_INT_24_8;
			break;
		default:
			// NOT SUPPORTED
			ENSURE_NO_ENTRY();
			return;
	}

	glGenTextures(1, &Id);
	
	if(InSamples > 1)
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, Id);
		
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, InSamples, format, InWidth, InHeight, GL_TRUE);
		
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, Id);
		
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, InWidth, InHeight, 0, format, type, nullptr);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	

	Width = InWidth;
	Height = InHeight;
	Type = InType;
	Samples = InSamples;
}

FRenderTexture::~FRenderTexture()
{
	if(IsInitialized())
		glDeleteTextures(1, &Id);
}

bool FRenderTexture::AttachFramebuffer(const uint8 UseIndex)
{
	if(!IsInitialized() || bIsAttached)
	{
		ENSURE_NO_ENTRY();
		return false;
	}
	
	if(UseIndex > 0 && Type != ERenderTargetAttachmentType::Color) return false;

	GLenum attachment;
	switch (Type)
	{
		case ERenderTargetAttachmentType::Color:
			attachment = GL_COLOR_ATTACHMENT0 + UseIndex;
			break;
		case ERenderTargetAttachmentType::DepthOnly:
			attachment = GL_DEPTH_ATTACHMENT;
			break;
		case ERenderTargetAttachmentType::DepthAndStencil:
			attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			break;
		default:
			ENSURE_NO_ENTRY();
			return false;
	}
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, (Samples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, Id, 0);
	bIsAttached = true;
	
	return true;
}

bool FRenderTexture::DetachFramebuffer()
{
	if(!IsInitialized() || !bIsAttached)
	{
		ENSURE_NO_ENTRY();
		return false;
	}

	bIsAttached = false;
	return true;
}