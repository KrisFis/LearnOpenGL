
#include "RenderTexture.h"
#include "Framebuffer.h"

FRenderTexture::FRenderTexture(uint8 InSamples, uint16 InWidth, uint16 InHeight, ERenderTargetType InType)
	: Id(0)
	, Type(ERenderTargetType::Invalid)
	, FBType(0)
	, Width(0)
	, Height(0)
	, Samples(0)
{
	GLenum internalFormat, format, type;
	switch (InType)
	{
		case ERenderTargetType::Color:
			internalFormat = format = GL_RGB;
			type = GL_UNSIGNED_BYTE;
			break;
		case ERenderTargetType::DepthOnly:
			internalFormat = format = GL_DEPTH_COMPONENT;
			type = GL_UNSIGNED_INT;
			break;
		case ERenderTargetType::DepthAndStencil:
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

bool FRenderTexture::AttachFramebuffer(const EFramebufferType FBTarget, const uint8 UseIndex)
{
	if(!IsInitialized() || FBType != 0)
	{
		ENSURE_NO_ENTRY();
		return false;
	}
	
	if(UseIndex > 0 && Type != ERenderTargetType::Color) return false;

	GLenum attachment;
	switch (Type)
	{
		case ERenderTargetType::Color:
			attachment = GL_COLOR_ATTACHMENT0 + UseIndex;
			break;
		case ERenderTargetType::DepthOnly:
			attachment = GL_DEPTH_ATTACHMENT;
			break;
		case ERenderTargetType::DepthAndStencil:
			attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			break;
		default:
			ENSURE_NO_ENTRY();
			return false;
	}
	
	glFramebufferTexture2D(FBTarget, attachment, (Samples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, Id, 0);
	FBType = FBTarget;
	
	return true;
}

bool FRenderTexture::DetachFramebuffer()
{
	if(!IsInitialized() || FBType == 0)
	{
		ENSURE_NO_ENTRY();
		return false;
	}

	FBType = 0;
	return true;
}