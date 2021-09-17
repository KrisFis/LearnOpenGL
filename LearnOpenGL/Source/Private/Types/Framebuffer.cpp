
#include "Framebuffer.h"
#include "RenderTexture.h"
#include "RenderBuffer.h"

FFramebuffer::FFramebuffer()
	: Id(0)
	, Target(0)
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
		glBindFramebuffer(Target, 0);
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

void FFramebuffer::Attach(const EFramebufferTarget InTargetType, const FRenderTexturePtr& InTexTarget)
{	
	if(!AttachImpl(InTargetType, InTexTarget.get()))
	{
		assert(false);
		return;
	}
	
	AddUniqueTarget(InTargetType);
	TexTargets.push_back(InTexTarget);
}

void FFramebuffer::Attach(const EFramebufferTarget InTargetType, const FRenderBufferPtr& InBuffTarget)
{
	if(!AttachImpl(InTargetType, InBuffTarget.get()))
	{
		assert(false);
		return;
	}

	AddUniqueTarget(InTargetType);
	BuffTargets.push_back(InBuffTarget);
}

bool FFramebuffer::AttachImpl(const EFramebufferTarget InTargetType, IRenderTarget* InTarget)
{
	if(!bIsInitialized || !InTarget) return false;
	
	glBindFramebuffer(InTargetType, Id);
	const bool result = InTarget->AttachFramebuffer(InTargetType);
	glBindFramebuffer(InTargetType, 0);
	
	return result;
}

void FFramebuffer::AddUniqueTarget(const EFramebufferTarget InTargetType)
{
	bool found = false;
	for(auto targetType : UsedTargetTypes)
		if(targetType == InTargetType) found = true;
		
	if(!found)
		UsedTargetTypes.push_back(InTargetType);
}