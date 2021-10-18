
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
	FORCEINLINE EFramebufferType GetType() const { return Type; }

public: // Use methods

	void Use();
	void Clear();

public: // Attach methods

	void Attach(const EFramebufferType InTargetType, const FRenderTargetPtr& InTarget);

private: // Helper methods

	bool AttachImpl(const EFramebufferType InTargetType, IRenderTarget* InTarget);
	void AddUniqueTarget(const EFramebufferType InTargetType);

private: // Fields

	FFramebufferId Id;
	EFramebufferType Type;

private: // Targets

	std::vector<EFramebufferType> UsedTargetTypes;
	std::vector<FRenderTargetPtr> RenderTargets;

private: // Primitive fields

	uint8 bIsUsed : 1;
	uint8 bIsInitialized : 1;
};

typedef TSharedPtr<FFramebuffer> FFramebufferPtr;