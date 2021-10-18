
#include "Framebuffer.h"
#include "RenderTexture.h"
#include "RenderBuffer.h"

FFramebuffer::FFramebuffer()
	: Id(0)
	, bIsUsed(false)
	, bIsInitialized(false)
{
	glGenFramebuffers(1, &Id);
	bIsInitialized = true;
}

FFramebuffer::~FFramebuffer()
{
	Clear();
	
	glDeleteFramebuffers(1, &Id);
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

void FFramebuffer::Attach(const EFramebufferType InTargetType, const FRenderTargetPtr& InTarget, bool Overwrite)
{	
	if(!AttachImpl(InTargetType, InTarget, Overwrite))
	{
		assert(false);
		return;
	}
	
	RenderTargets.push_back(InTarget);
}

bool FFramebuffer::AttachImpl(const EFramebufferType InTargetType, const FRenderTargetPtr& InTarget, bool Overwrite)
{
	if(!bIsInitialized || !InTarget.IsValid()) return false;
	
	{
		auto getActionType =
		[](EFramebufferType Type) -> uint8
		{
			enum { Read = 0x1, Draw = 0x2 }; 
			
			if(Type == GL_FRAMEBUFFER) return (Read | Draw);
			else if(Type == GL_READ_FRAMEBUFFER) return Read;
			else if(Type == GL_DRAW_FRAMEBUFFER) return Draw;
			else return 0;
		};
	
		const uint8 wantsActions = getActionType(InTargetType);
		for(auto it = UsedTargetTypes.begin(); it != UsedTargetTypes.end(); ++it)
		{
			const uint8 usedAction = getActionType(*it);
			if(wantsActions & usedAction)
			{
				if(Overwrite)
					UsedTargetTypes.erase(it);
				else
					return false;
			}
		}
	}
	
	glBindFramebuffer(InTargetType, Id);
	const bool result = InTarget->AttachFramebuffer(InTargetType);
	glBindFramebuffer(InTargetType, 0);
	
	return result;
}