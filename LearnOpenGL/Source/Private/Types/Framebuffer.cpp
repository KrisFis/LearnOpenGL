
#include "Framebuffer.h"
#include "RenderTexture.h"
#include "RenderBuffer.h"

FFramebuffer::FFramebuffer()
	: Id(0)
	, bIsEnabled(false)
	, bHasChanges(true)
{}

FFramebuffer::~FFramebuffer()
{
	Disable();
	
	if(Id != NRenderConsts::Default::FramebufferId)
	{
		glDeleteFramebuffers(1, &Id);
	}
}

TArray<FRenderTargetPtr> FFramebuffer::GetAttachments(const ERenderTargetType Type) const
{
	auto foundTargets = RenderTargets.find(Type);
	if(foundTargets == RenderTargets.end())
		return {};
		
	return foundTargets->second;
}

void FFramebuffer::Enable()
{
	if(bIsEnabled) return;

	for(auto targetType : UsedFBTypes)
	{
		glBindFramebuffer(targetType.first, Id);
	}

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ENSURE_NO_ENTRY();
		
		for(auto targetType : UsedFBTypes)
		{
			glBindFramebuffer(targetType.first, 0);
		}
	}
	else
	{
		bIsEnabled = true;
		bHasChanges = false;
	}
}

void FFramebuffer::Disable()
{
	if(!bIsEnabled) return;
	
	for(auto targetType : UsedFBTypes)
		glBindFramebuffer(targetType.first, 0);

	bIsEnabled = false;
}

bool FFramebuffer::Attach(const EFramebufferType FBTargetType, const FRenderTargetPtr& Target)
{	
	if(!Target.IsValid()) return false;

	// Generate id if necessary
	if(Id == NRenderConsts::Default::FramebufferId)
	{
		glGenFramebuffers(1, &Id);
	}
	
	const ERenderTargetType targetType = Target->GetType();
	auto foundTargets = RenderTargets.find(targetType);
	const uint8 useIndex = (foundTargets == RenderTargets.end()) ? 0 : (uint8)foundTargets->second.size();
	
	glBindFramebuffer(FBTargetType, Id);
	ENSURE_RET(Target->AttachFramebuffer(FBTargetType, useIndex), false);
	glBindFramebuffer(FBTargetType, 0);
	
	auto foundFbType = UsedFBTypes.find(FBTargetType);
	if(foundFbType == UsedFBTypes.end())
		UsedFBTypes.insert({FBTargetType, 1});
	else
		++foundFbType->second;
	
	if(foundTargets == RenderTargets.end())
		RenderTargets.insert({targetType, {Target}});
	else
		foundTargets->second.push_back(Target);
	
	bHasChanges = true;
	
	return true;
}

void FFramebuffer::DetachAll()
{
	for(auto& targetArr : RenderTargets)
	{
		for(auto& target : targetArr.second)
		{
			target->DetachFramebuffer();
		}
	}
	
	UsedFBTypes.clear();
	RenderTargets.clear();
	
	bHasChanges = true;
}

void FFramebuffer::DetachAll(ERenderTargetType Type)
{
	auto foundArr = RenderTargets.find(Type);
	if(foundArr == RenderTargets.end()) return;
	
	for(auto& target : foundArr->second)
	{
		const EFramebufferType attachFbType = target->GetAttachedFBType();
	
		auto foundFbType = UsedFBTypes.find(attachFbType);
		--foundFbType->second;
		
		if(foundFbType->second == 0)
		{
			UsedFBTypes.erase(foundFbType);
		}
			
		target->DetachFramebuffer();
	}
	
	RenderTargets.erase(foundArr);
	
	bHasChanges = true;
}

bool FFramebuffer::Detach(const FRenderTargetPtr& Target)
{
	ENSURE_VALID_RET(Target, false);
	
	auto foundTargetArr = RenderTargets.find(Target->GetType());
	if(foundTargetArr == RenderTargets.end()) return false;
	
	auto foundTarget = std::find(foundTargetArr->second.begin(), foundTargetArr->second.end(), Target);
	if(foundTarget == foundTargetArr->second.end()) return false;
	
	if(foundTargetArr->second.size() > 1)
	{
		foundTargetArr->second.erase(foundTarget);
	}
	else
	{
		RenderTargets.erase(foundTargetArr);
	}
	
	auto foundFbType = UsedFBTypes.find(Target->GetAttachedFBType());
	--foundFbType->second;
	
	if(foundFbType->second == 0)
	{
		UsedFBTypes.erase(foundFbType);
	}
	
	Target->DetachFramebuffer();
	
	bHasChanges = true;
	return true;
}
