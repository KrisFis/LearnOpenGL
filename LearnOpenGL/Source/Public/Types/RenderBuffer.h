
#pragma once

#include "ModuleMinimal.h"
#include "RenderTarget.h"

class FRenderBuffer : public IRenderTarget
{
	
public: // Constructors

	FRenderBuffer(uint16 InWidth, uint16 InHeight, ERenderTargetType InType);
	virtual ~FRenderBuffer();
	
public: // IRenderTarget overrides

	inline virtual bool IsInitialized() const override { return Type != ERenderTargetType::Invalid; };
	inline virtual bool IsAttached() const override { return Target != 0; };

protected: // IRenderTarget overrides

	virtual bool AttachFramebuffer(const EFramebufferTarget FBTarget) override;
	virtual bool DetachFramebuffer() override;

private: // Fields

	FRenderBufferId Id;
	ERenderTargetType Type;
	EFramebufferTarget Target;

	uint16 Width;
	uint16 Height;
};