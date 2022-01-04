
#include "RenderBuffer.h"

FRenderBuffer::FRenderBuffer(uint8 InSamples, uint16 InWidth, uint16 InHeight, ERenderTargetAttachmentType InType)
	: Id(0)
	, Type(ERenderTargetAttachmentType::Invalid)
	, Width(0)
	, Height(0)
	, Samples(0)
	, bIsAttached(false)
{
	GLenum internalFormat;
	switch (InType)
	{
		case ERenderTargetAttachmentType::DepthOnly:
			internalFormat = GL_DEPTH_COMPONENT;
			break;
		case ERenderTargetAttachmentType::DepthAndStencil:
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

bool FRenderBuffer::AttachFramebuffer(const uint8 UseIndex)
{
	if(!IsInitialized() || bIsAttached)
	{
		ENSURE_NO_ENTRY();
		return false;
	}

	if(UseIndex > 0) return false;

	GLenum internalFormat;
	switch (Type)
	{
		case ERenderTargetAttachmentType::DepthOnly:
			internalFormat = GL_DEPTH_ATTACHMENT;
			break;
		case ERenderTargetAttachmentType::DepthAndStencil:
			internalFormat = GL_DEPTH_STENCIL_ATTACHMENT;
			break;
		default:
			ENSURE_NO_ENTRY();
			return false;
	}
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, internalFormat, GL_RENDERBUFFER, Id);
	bIsAttached = true;
	
	return true;
}

bool FRenderBuffer::DetachFramebuffer()
{
	if(!IsInitialized() || !bIsAttached)
	{
		ENSURE_NO_ENTRY();
		return false;
	}
	
	bIsAttached = false;
	return true;
}