
#include "RenderBuffer.h"

FRenderBuffer::FRenderBuffer(uint16 InWidth, uint16 InHeight, ERenderTargetType InType)
	: Id(0)
	, Type(ERenderTargetType::Invalid)
	, FBType(0)
	, Width(0)
	, Height(0)
{
	GLenum internalFormat;
	switch (InType)
	{
		case ERenderTargetType::DepthOnly:
			internalFormat = GL_DEPTH_COMPONENT;
			break;
		case ERenderTargetType::DepthAndStencil:
			internalFormat = GL_DEPTH24_STENCIL8;
			break;
		default:
			ENSURE_NO_ENTRY();
			return;
	}
	
	glGenRenderbuffers(1, &Id);
	glBindRenderbuffer(GL_RENDERBUFFER, Id);
	glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, InWidth, InHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	Width = InWidth;
	Height = InHeight;
	Type = InType;
}

FRenderBuffer::~FRenderBuffer()
{
	if(IsInitialized())
		glDeleteRenderbuffers(1, &Id);
}

bool FRenderBuffer::AttachFramebuffer(const EFramebufferType FBTarget)
{
	if(!IsInitialized() || IsAttached())
	{
		ENSURE_NO_ENTRY();
		return false;
	}

	GLenum internalFormat;
	switch (Type)
	{
		case ERenderTargetType::DepthOnly:
			internalFormat = GL_DEPTH_ATTACHMENT;
			break;
		case ERenderTargetType::DepthAndStencil:
			internalFormat = GL_DEPTH_STENCIL_ATTACHMENT;
			break;
		default:
			ENSURE_NO_ENTRY();
			return false;
	}
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, internalFormat, GL_RENDERBUFFER, Id);
	FBType = FBTarget;
	
	return true;
}

bool FRenderBuffer::DetachFramebuffer()
{
	if(!IsInitialized() || !IsAttached())
	{
		ENSURE_NO_ENTRY();
		return false;
	}
	
	FBType = 0;
	return true;
}
