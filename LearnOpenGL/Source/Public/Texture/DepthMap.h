
#pragma once

#include "ModuleMinimal.h"

// Forward declaration
class FFramebuffer;

class FDepthMap
{
private: // Private constructors

	FDepthMap(uint16 InWidth, uint16 InHeight);

public: // Destructor 

	virtual ~FDepthMap();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FDepthMap> Create(uint16 Width, uint16 Height)
	{ return MakeShareable(new FDepthMap(Width, Height)); }

public: // External methods

	void Enable();
	void Disable();

private: // Cache

	uint16 LastKnownWidth, LastKnownHeight;

private: // Fields

	FTextureId TexId;
	FFramebufferId FBId;
	
	uint16 Width, Height;

private: // Primitive fields

	uint8 bIsEnabled : 1;
};

typedef TSharedPtr<FDepthMap> FDepthMapPtr;