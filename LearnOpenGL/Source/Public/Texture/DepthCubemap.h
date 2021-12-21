
#pragma once

#include "ModuleMinimal.h"

class FDepthCubemap
{
private: // Constructor

	FDepthCubemap(uint16 InWidth, uint16 InHeight);
	
public: // Destructor

	~FDepthCubemap();

public: // Static construction

	FORCEINLINE static TSharedPtr<FDepthCubemap> Create(uint16 Width, uint16 Height) 
	{	return MakeShareable(new FDepthCubemap(Width, Height)); }

public: // Getters

	FORCEINLINE FTextureId GetCubeId() const { return CubeId; }
	FORCEINLINE FFramebufferId GetFBId() const { return FBId; }

	FORCEINLINE bool IsEnabled() const { return bIsEnabled; }

public: // External methods

	void Enable();
	void Disable();

private: // Fields

	FTextureId CubeId;
	FFramebufferId FBId;

	uint16 Width, Height;

private: // Primitive fields

	uint8 bIsEnabled : 1;
	
};

typedef TSharedPtr<FDepthCubemap> FDepthCubemapPtr;