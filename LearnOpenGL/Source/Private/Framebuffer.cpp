
#include "Framebuffer.h"
#include "RenderTexture.h"
#include "RenderBuffer.h"

FFramebuffer::FFramebuffer()
	: Id(0)
	, bIsEnabled(false)
	, bHasChanges(true)
{
	glGenFramebuffers(1, &Id);
}

FFramebuffer::~FFramebuffer()
{
	Disable();
	
	glDeleteFramebuffers(1, &Id);
}

TArray<FRenderTargetPtr> FFramebuffer::GetAttachments(const ERenderTargetType Type) const
{
	auto foundTargets = Targets.find(Type);
	if(foundTargets == Targets.end())
		return {};
		
	return foundTargets->second;
}

void FFramebuffer::Enable()
{
	glBindFramebuffer(GL_FRAMEBUFFER, Id);
	
	uint8 numOfColors = 0;
	uint8 numOfDepth = 0;
	
	for(const auto& pair : Targets)
	{
		switch (pair.first)
		{
			case ERenderTargetType::Color:
			numOfColors += pair.second.size();
			break;
			case ERenderTargetType::DepthOnly:
			case ERenderTargetType::DepthAndStencil:
			numOfDepth += pair.second.size();
			break;
			default:
			break;
		}
	}

	switch (numOfColors)
	{
		case 0: // DEPTH ONLY PASS
		{
			if(numOfDepth > 0)
			{
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
			}
		}
		break;
		case 1: // DEFAULT SETUP
		break;
		default:
		{
			TArray<GLenum> colorAttachments;
			colorAttachments.reserve(numOfColors);
			
			for(uint8 i = 0; i < numOfColors; ++i)
			{
				colorAttachments.push_back(GL_COLOR_ATTACHMENT0+i);
			}
			
			glDrawBuffers(numOfColors, colorAttachments.data());
		}
		break;
	}

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ENSURE_NO_ENTRY();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return;
	}
	
	bIsEnabled = true;
	bHasChanges = false;
}

void FFramebuffer::Disable()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	bIsEnabled = false;
}

bool FFramebuffer::CopyTo(const TSharedPtr<FFramebuffer>& Destination, const FFramebufferCopyArgs& CopyArgs) const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, Id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Destination->Id);
	
	GLenum dataType;
	switch(CopyArgs.DataType)
	{
		case FFramebufferCopyArgs::DT_Color:
			dataType = GL_COLOR_BUFFER_BIT;
			break;
		case FFramebufferCopyArgs::DT_Depth:
			dataType = GL_DEPTH_BUFFER_BIT;
			break;
		case FFramebufferCopyArgs::DT_Stencil:
			dataType = GL_STENCIL_BUFFER_BIT;
			break;
		default:
			ENSURE_NO_ENTRY();
			return false;
	}
	
	GLenum filterType;
	switch (CopyArgs.FilterType)
	{
		case FFramebufferCopyArgs::FT_Nearest:
			filterType = GL_NEAREST;
			break;
		case FFramebufferCopyArgs::FT_Linear:
			filterType = GL_LINEAR;
			break;
		default:
			ENSURE_NO_ENTRY();
			return false;
	}
	
	glBlitFramebuffer(
		CopyArgs.Source.Pos.x, CopyArgs.Source.Pos.y, CopyArgs.Source.Size.x, CopyArgs.Source.Size.y, 
		CopyArgs.Destination.Pos.x, CopyArgs.Destination.Pos.y, CopyArgs.Destination.Size.x, CopyArgs.Destination.Size.y,
		dataType, 
		filterType
	);
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	
	return true;
}

int16 FFramebuffer::Attach(const FRenderTargetPtr& Target)
{	
	if(!Target.IsValid()) return -1;
	
	const ERenderTargetType targetType = Target->GetType();
	auto foundTargets = Targets.find(targetType);
	const uint8 useIndex = (foundTargets == Targets.end()) ? 0 : (uint8)foundTargets->second.size();
	
	glBindFramebuffer(GL_FRAMEBUFFER, Id);
	ENSURE_RET(Target->AttachFramebuffer(useIndex), -1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	if(foundTargets == Targets.end())
		Targets.insert({targetType, {Target}});
	else
		foundTargets->second.push_back(Target);
	
	bHasChanges = true;
	
	return useIndex;
}

void FFramebuffer::DetachAll()
{
	for(auto& targetArr : Targets)
	{
		for(auto& target : targetArr.second)
		{
			target->DetachFramebuffer();
		}
	}
	
	Targets.clear();
	
	bHasChanges = true;
}

void FFramebuffer::DetachAll(ERenderTargetType Type)
{
	auto foundArr = Targets.find(Type);
	if(foundArr == Targets.end()) return;
	
	for(auto& target : foundArr->second)
	{
		target->DetachFramebuffer();
	}
	
	Targets.erase(foundArr);
	
	bHasChanges = true;
}

bool FFramebuffer::Detach(const FRenderTargetPtr& Target)
{
	ENSURE_VALID_RET(Target, false);
	
	auto foundTargetArr = Targets.find(Target->GetType());
	if(foundTargetArr == Targets.end()) return false;
	
	auto foundTarget = std::find(foundTargetArr->second.begin(), foundTargetArr->second.end(), Target);
	if(foundTarget == foundTargetArr->second.end()) return false;
	
	if(foundTargetArr->second.size() > 1)
	{
		foundTargetArr->second.erase(foundTarget);
	}
	else
	{
		Targets.erase(foundTargetArr);
	}
	
	Target->DetachFramebuffer();
	
	bHasChanges = true;
	return true;
}
