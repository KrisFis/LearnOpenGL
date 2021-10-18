
#pragma once

#include "ModuleMinimal.h"
#include "RenderTarget.h"

class FRenderBuffer : public IRenderTarget
{
	
public: // Constructors

	FRenderBuffer(uint16 InWidth, uint16 InHeight, ERenderTargetType InType);
	virtual ~FRenderBuffer();
	
public: // IRenderTarget overrides

	FORCEINLINE virtual bool IsInitialized() const override { return Type != ERenderTargetType::Invalid; };
	FORCEINLINE virtual bool IsAttached() const override { return FBType != 0; };

protected: // IRenderTarget overrides

	virtual bool AttachFramebuffer(const EFramebufferType FBTarget) override;
	virtual bool DetachFramebuffer() override;

private: // Fields

	FRenderBufferId Id;
	ERenderTargetType Type;
	EFramebufferType FBType;

	uint16 Width;
	uint16 Height;
};