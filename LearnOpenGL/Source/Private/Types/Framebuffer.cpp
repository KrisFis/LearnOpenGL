
#include "Framebuffer.h"
#include "RenderTexture.h"
#include "RenderBuffer.h"

FFramebuffer::FFramebuffer()
	: Id(0)
	, Type(0)
	, bIsUsed(false)
	, bIsInitialized(false)
{
	glGenFramebuffers(1, &Id);
	bIsInitialized = true;
}

FFramebuffer::~FFramebuffer()
{
	glDeleteFramebuffers(1, &Id);
	
	if(bIsUsed)
	{
		// Assign default framebuffer
		glBindFramebuffer(Type, 0);
	}
}

void FFramebuffer::Use()
{
	if(bIsUsed) return;

	for(auto targetType : UsedTargetTypes)
	{
		glBindFramebuffer(targetType, Id);
	}

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		for(auto targetType : UsedTargetTypes)
		{
			glBindFramebuffer(targetType, 0);
		}
	}
	else
	{
		bIsUsed = true;
	}
}

void FFramebuffer::Clear()
{
	if(!bIsUsed) return;
	
	for(auto targetType : UsedTargetTypes)
		glBindFramebuffer(targetType, 0);
		
	bIsUsed = false;
}

void FFramebuffer::Attach(const EFramebufferType InTargetType, const FRenderTargetPtr& InTarget)
{	
	if(!AttachImpl(InTargetType, InTarget.Get()))
	{
		assert(false);
		return;
	}
	
	AddUniqueTarget(InTargetType);
	RenderTargets.push_back(InTarget);
}

bool FFramebuffer::AttachImpl(const EFramebufferType InTargetType, IRenderTarget* InTarget)
{
	if(!bIsInitialized || !InTarget) return false;
	
	glBindFramebuffer(InTargetType, Id);
	const bool result = InTarget->AttachFramebuffer(InTargetType);
	glBindFramebuffer(InTargetType, 0);
	
	return result;
}

void FFramebuffer::AddUniqueTarget(const EFramebufferType InTargetType)
{
	bool found = false;
	for(auto targetType : UsedTargetTypes)
		if(targetType == InTargetType) found = true;
		
	if(!found)
		UsedTargetTypes.push_back(InTargetType);
}