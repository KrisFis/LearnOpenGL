
#pragma once

#include "ModuleMinimal.h"

// Forward declaration
class IRenderTarget;

class FFramebuffer
{
	
public: // Constructors

	FFramebuffer();
	virtual ~FFramebuffer();

public: // Getters

	inline bool IsInitialized() const { return bIsInitialized; }
	inline bool IsUsed() const { return bIsUsed; }
	
	inline FFramebufferId GetId() const { return Id; }
	inline EFramebufferType GetType() const { return Type; }

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