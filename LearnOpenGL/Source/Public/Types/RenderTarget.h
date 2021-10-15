
#pragma once

#include "ModuleMinimal.h"

enum class ERenderTargetType : uint8
{
	Invalid,
	Color,
	DepthOnly,
	DepthAndStencil
};

class IRenderTarget
{

public: // Getters

	virtual bool IsInitialized() const = 0;
	virtual bool IsAttached() const = 0;
	virtual ERenderTargetType GetType() const = 0;

protected: // Setup

	friend class FFramebuffer;

	virtual bool AttachFramebuffer(const EFramebufferType FBTarget) = 0;
	virtual bool DetachFramebuffer() = 0;
};