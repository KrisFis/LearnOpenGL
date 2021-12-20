
#include "DepthMap.h"

FDepthMap::FDepthMap(uint16 InWidth, uint16 InHeight)
	: UsedTexIndex(-1)
	, TexId(0)
	, FBId(0)
	, Width(InWidth)
	, Height(InHeight)
	, bIsEnabled(false)
{

	// Is here because of badly implemented framebuffer
	// * TODO(kristian.fisera): Fix framebuffer

	// Texture
	{
		glGenTextures(1, &TexId);
		glBindTexture(GL_TEXTURE_2D, TexId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
	}
	
	// Framebuffer
	{
		glGenFramebuffers(1, &FBId);
		glBindFramebuffer(GL_FRAMEBUFFER, FBId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TexId, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  
	}
}

FDepthMap::~FDepthMap() {}

void FDepthMap::Enable()
{
	if(bIsEnabled) return;
	
	FApplication& app = FApplication::Get();
	
	app.GetWindowSize(LastKnownWidth, LastKnownHeight);
	app.SetWindowSize(Width, Height);

	glCullFace(GL_FRONT);
	glBindFramebuffer(GL_FRAMEBUFFER, FBId);
	glCullFace(GL_BACK);
	
	bIsEnabled = true;
}

void FDepthMap::Disable()
{
	if(!bIsEnabled) return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	FApplication& app = FApplication::Get();
	app.SetWindowSize(LastKnownWidth, LastKnownHeight);
	
	bIsEnabled = false;
}

void FDepthMap::UseTexture(const uint8 Index)
{
	glActiveTexture(GL_TEXTURE0 + Index);
	glBindTexture(GL_TEXTURE_2D, TexId);
	
	UsedTexIndex = Index;
}

void FDepthMap::ClearTexture()
{
	if(UsedTexIndex == -1) return;

	glBindTexture(GL_TEXTURE_2D, 0);
	
	UsedTexIndex = -1;
}
