
#pragma once

#include "ModuleMinimal.h"

// Forward declaration
class IRenderTarget;

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

	FORCEINLINE bool IsInitialized() const { return bIsInitialized; }
	FORCEINLINE bool IsUsed() const { return bIsUsed; }
	
	FORCEINLINE FFramebufferId GetId() const { return Id; }

public: // Use methods

	void Use();
	void Clear();

public: // Attach methods

	void Attach(const EFramebufferType InTargetType, const FRenderTargetPtr& InTarget, bool Overwrite = true);

private: // Helper methods

	bool AttachImpl(const EFramebufferType InTargetType, const FRenderTargetPtr& InTarget, bool Overwrite);

private: // Fields

	FFramebufferId Id;

private: // Targets

	std::vector<EFramebufferType> UsedTargetTypes;
	std::vector<FRenderTargetPtr> RenderTargets;

private: // Primitive fields

	uint8 bIsUsed : 1;
	uint8 bIsInitialized : 1;
};

typedef TSharedPtr<FFramebuffer> FFramebufferPtr;