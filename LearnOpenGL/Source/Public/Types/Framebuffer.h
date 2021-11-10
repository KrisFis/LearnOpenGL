
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
	FORCEINLINE bool HasChanges() const { return bHasChanges; }
	FORCEINLINE FFramebufferId GetId() const { return Id; }
	
public: // Getters -> Attachment

	FORCEINLINE FRenderTargetPtr GetFirstAttachment(const ERenderTargetType Type) const { return GetAttachments(Type)[0]; }
	TArray<FRenderTargetPtr> GetAttachments(const ERenderTargetType Type) const;

public: // Use methods

	void Enable();
	void Disable();

public: // Attach methods

	bool Attach(const EFramebufferType FBTargetType, const FRenderTargetPtr& Target);
	
	void DetachAll();
	void DetachAll(ERenderTargetType Type);
	bool Detach(const FRenderTargetPtr& Target);

private: // Fields

	FFramebufferId Id;

private: // Targets

	TMap<EFramebufferType, uint8> UsedFBTypes;
	TFastMap<ERenderTargetType, TArray<FRenderTargetPtr>> RenderTargets;

private: // Primitive fields

	uint8 bIsEnabled : 1;
	uint8 bHasChanges : 1;
};

typedef TSharedPtr<FFramebuffer> FFramebufferPtr;