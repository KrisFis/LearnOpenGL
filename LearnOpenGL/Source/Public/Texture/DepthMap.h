
#pragma once

#include "ModuleMinimal.h"

// Forward declaration
class FFramebuffer;

// TODO(kristian.fisera): Use Framebuffer internally
class FDepthMap
{
private: // Private constructors

	 FDepthMap(uint16 InWidth, uint16 InHeight);

public: // Destructor 

	virtual ~FDepthMap();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FDepthMap> Create(uint16 Width, uint16 Height)
	{ return MakeShareable(new FDepthMap(Width, Height)); }

public: // Getters

	FORCEINLINE FTextureId GetTextureId() const { return TexId; }
	FORCEINLINE FFramebufferId GetFBId() const { return FBId; }

	FORCEINLINE bool IsEnabled() const { return bIsEnabled; }
	FORCEINLINE int16 GetBoundTextureIndex() const { return UsedTexIndex; }

public: // External methods

	void Enable();
	void Disable();

	void UseTexture(const uint8 Index);
	void ClearTexture();

private: // Cache

	uint16 LastKnownWidth, LastKnownHeight;
	int16 UsedTexIndex;

private: // Fields

	FTextureId TexId;
	FFramebufferId FBId;
	
	uint16 Width, Height;

private: // Primitive fields

	uint8 bIsEnabled : 1;
};

typedef TSharedPtr<FDepthMap> FDepthMapPtr;