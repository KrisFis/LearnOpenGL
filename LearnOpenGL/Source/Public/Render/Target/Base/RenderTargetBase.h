
#pragma once

#include "ModuleMinimal.h"

enum class ERenderTargetAttachmentType : uint8
{
	Invalid,
	Color,
	DepthOnly,
	DepthAndStencil
};

enum class ERenderTargetType : uint8
{
	Invalid,
	RenderTexture, // FRenderFTexture
	RenderBuffer // FRenderBuffer
};

class IRenderTarget : public TSharedClass<IRenderTarget>
{

public: // Getters

	virtual bool IsInitialized() const = 0;
	virtual ERenderTargetType GetType() const = 0;
	virtual ERenderTargetAttachmentType GetAttachmentType() const = 0;
	virtual bool IsAttached() const = 0;
	virtual uint8 GetSamples() const = 0;

protected: // Setup

	friend class FFramebuffer;

	virtual bool AttachFramebuffer(const uint8 UseIndex) = 0;
	virtual bool DetachFramebuffer() = 0;
};

typedef TSharedPtr<IRenderTarget> FRenderTargetPtr;