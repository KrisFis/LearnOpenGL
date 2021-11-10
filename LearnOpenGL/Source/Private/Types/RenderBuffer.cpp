
#include "RenderBuffer.h"

FRenderBuffer::FRenderBuffer(uint8 InSamples, uint16 InWidth, uint16 InHeight, ERenderTargetType InType)
	: Id(0)
	, Type(ERenderTargetType::Invalid)
	, FBType(0)
	, Width(0)
	, Height(0)
	, Samples(0)
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
	
	if(InSamples > 1)
	{
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, InSamples, internalFormat, InWidth, InHeight);
	}
	else
	{
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, InWidth, InHeight);
	}
	
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	Width = InWidth;
	Height = InHeight;
	Type = InType;
	Samples = InSamples;
}

FRenderBuffer::~FRenderBuffer()
{
	if(IsInitialized())
		glDeleteRenderbuffers(1, &Id);
}

bool FRenderBuffer::AttachFramebuffer(const EFramebufferType FBTarget, const uint8 UseIndex)
{
	if(!IsInitialized() || FBType != 0)
	{
		ENSURE_NO_ENTRY();
		return false;
	}

	if(UseIndex > 0) return false;

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
	if(!IsInitialized() || FBType == 0)
	{
		ENSURE_NO_ENTRY();
		return false;
	}
	
	FBType = 0;
	return true;
}