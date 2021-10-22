
#pragma once

#include "ModuleMinimal.h"

// Forward declaration
class IRenderTarget;
enum class ERenderTargetType : uint8;

class FFramebuffer
{
	
private: // Constructors

	explicit FFramebuffer();
	
public: // Destructor
	
	virtual ~FFramebuffer();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FFramebuffer> Create()
	{ return MakeShareable(new FFramebuffer()); }

public: // Getters

	FORCEINLINE bool IsUsed() const { return bIsUsed; }
	FORCEINLINE FFramebufferId GetId() const { return Id; }

public: // Use methods

	void Use();
	void Clear();

public: // Attach methods

	bool Attach(const EFramebufferType FBTargetType, const FRenderTargetPtr& Target, bool Overwrite = true);

private: // Fields

	FFramebufferId Id;

private: // Targets

	TArray<EFramebufferType> UsedFBTypes;
	TFastMap<ERenderTargetType, FRenderTargetPtr> RenderTargets;

private: // Primitive fields

	uint8 bIsUsed : 1;
};

typedef TSharedPtr<FFramebuffer> FFramebufferPtr;