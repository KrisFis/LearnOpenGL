
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

	FORCEINLINE bool IsEnabled() const { return bIsEnabled; }
	FORCEINLINE FFramebufferId GetId() const { return Id; }
	
public: // Getters -> Attachment

	FRenderTargetPtr GetAttachments(const ERenderTargetType Type) const;

public: // Use methods

	void Enable();
	void Disable();

public: // Attach methods

	bool Attach(const EFramebufferType FBTargetType, const FRenderTargetPtr& Target, bool Overwrite = true);

private: // Fields

	FFramebufferId Id;

private: // Targets

	TArray<EFramebufferType> UsedFBTypes;
	TFastMap<ERenderTargetType, FRenderTargetPtr> RenderTargets;

private: // Primitive fields

	uint8 bIsEnabled : 1;
};

typedef TSharedPtr<FFramebuffer> FFramebufferPtr;