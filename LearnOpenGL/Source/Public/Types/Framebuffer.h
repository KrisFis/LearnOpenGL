
#pragma once

#include "ModuleMinimal.h"

class FFramebuffer
{
	
public: // Constructors

	FFramebuffer();
	virtual ~FFramebuffer();

public: // Getters

	inline bool IsInitialized() const { return bIsInitialized; }
	inline bool IsUsed() const { return bIsUsed; }
	
	inline FFramebufferId GetId() const { return Id; }
	inline EFramebufferTarget GetTarget() const { return Target; }

public: // Use methods

	void Use();
	void Clear();

public: // Attach methods

	void Attach(const EFramebufferTarget InTargetType, const FRenderTexturePtr& InTexTarget);
	void Attach(const EFramebufferTarget InTargetType, const FRenderBufferPtr& InBuffTarget);

private: // Helper methods

	bool AttachImpl(const EFramebufferTarget InTargetType, class IRenderTarget* InTarget);
	void AddUniqueTarget(const EFramebufferTarget InTargetType);

private: // Fields

	FFramebufferId Id;
	EFramebufferTarget Target;

private: // Targets

	std::vector<EFramebufferTarget> UsedTargetTypes;
	std::vector<FRenderTexturePtr> TexTargets;
	std::vector<FRenderBufferPtr> BuffTargets;

private: // Primitive fields

	uint8 bIsUsed : 1;
	uint8 bIsInitialized : 1;
};