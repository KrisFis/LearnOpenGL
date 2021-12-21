
#include "DepthCubemap.h"

FDepthCubemap::FDepthCubemap(uint16 InWidth, uint16 InHeight)
	: CubeId(0)
	, FBId(0)
	, Width(InWidth)
	, Height(InHeight)
	, UsedCubeIndex(-1)
	, bIsEnabled(false)
{
	
	glGenTextures(1, &CubeId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeId);
	
	for(uint8 i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  
	
	glGenFramebuffers(1, &FBId);
	glBindFramebuffer(GL_FRAMEBUFFER, FBId);
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, CubeId, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FDepthCubemap::~FDepthCubemap()
{}

void FDepthCubemap::Enable()
{
	FApplication& app = FApplication::Get();
	
	app.GetWindowSize(LastKnownWidth, LastKnownHeight);
	app.SetWindowSize(Width, Height);

	glBindFramebuffer(GL_FRAMEBUFFER, FBId);

	bIsEnabled = true;
}

void FDepthCubemap::Disable()
{
	if(!bIsEnabled) return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	FApplication& app = FApplication::Get();
	app.SetWindowSize(LastKnownWidth, LastKnownHeight);

	bIsEnabled = false;
}

void FDepthCubemap::UseCube(uint8 Index)
{
	glActiveTexture(GL_TEXTURE0 + Index);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeId);
	
	UsedCubeIndex = Index;
}

void FDepthCubemap::ClearCube()
{
	if(UsedCubeIndex == -1) return;
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	UsedCubeIndex = -1;
}
