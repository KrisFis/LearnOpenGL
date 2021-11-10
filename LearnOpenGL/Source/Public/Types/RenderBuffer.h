
#pragma once

#include "ModuleMinimal.h"
#include "RenderTarget.h"

class FRenderBuffer : public IRenderTarget
{

private: // Constructors

	explicit FRenderBuffer(uint8 InSamples, uint16 InWidth, uint16 InHeight, ERenderTargetType InType);

public: // Destructors
	
	virtual ~FRenderBuffer();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FRenderBuffer> Create(uint16 Width, uint16 Height, ERenderTargetType Type, uint8 Samples = 1)
	{ return MakeShareable(new FRenderBuffer(Samples, Width, Height, Type)); }
	
public: // IRenderTarget overrides

	FORCEINLINE virtual bool IsInitialized() const override { return Type != ERenderTargetType::Invalid; };
	FORCEINLINE virtual ERenderTargetType GetType() const override { return Type; }
	
	FORCEINLINE virtual EFramebufferType GetAttachedFBType() const override { return FBType; }
	FORCEINLINE virtual uint8 GetSamples() const override { return Samples; }

protected: // IRenderTarget overrides

	virtual bool AttachFramebuffer(const EFramebufferType FBTarget, const uint8 UseIndex) override;
	virtual bool DetachFramebuffer() override;

private: // Fields

	FRenderBufferId Id;
	ERenderTargetType Type;
	EFramebufferType FBType;

	uint16 Width;
	uint16 Height;
	
	uint8 Samples;
};