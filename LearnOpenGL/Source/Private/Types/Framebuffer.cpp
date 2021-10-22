
#include "Framebuffer.h"
#include "RenderTexture.h"
#include "RenderBuffer.h"
#include "../../Public/Types/Framebuffer.h"


FFramebuffer::FFramebuffer()
	: Id(0)
	, bIsUsed(false)
{}

FFramebuffer::~FFramebuffer()
{
	Clear();
	
	if(Id != NRenderConsts::Default::FramebufferId)
	{
		glDeleteFramebuffers(1, &Id);
	}
}

void FFramebuffer::Use()
{
	if(bIsUsed) return;

	for(auto targetType : UsedFBTypes)
	{
		glBindFramebuffer(targetType, Id);
	}

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		for(auto targetType : UsedFBTypes)
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
	
	for(auto targetType : UsedFBTypes)
		glBindFramebuffer(targetType, 0);
		
	bIsUsed = false;
}

bool FFramebuffer::Attach(const EFramebufferType FBTargetType, const FRenderTargetPtr& Target, bool Overwrite)
{	
	if(!Target.IsValid()) return false;

	// Generate id if necessary
	if(Id == NRenderConsts::Default::FramebufferId)
	{
		glGenFramebuffers(1, &Id);
	}

	const ERenderTargetType targetType = Target->GetType();
	for(auto it = RenderTargets.begin(); it != RenderTargets.end(); ++it)
	{
		if(targetType == it->first)
		{
			if(Overwrite)
				RenderTargets.erase(it);
			else
				return false;
		}
	}
	
	glBindFramebuffer(FBTargetType, Id);
	ENSURE_RET(Target->AttachFramebuffer(FBTargetType), false);
	glBindFramebuffer(FBTargetType, 0);
	
	auto foundUsedFBType = std::find(UsedFBTypes.begin(), UsedFBTypes.end(), FBTargetType);
	if(foundUsedFBType == UsedFBTypes.end())
		UsedFBTypes.push_back(FBTargetType);
	
	RenderTargets.insert({targetType, Target});
	
	return true;
}