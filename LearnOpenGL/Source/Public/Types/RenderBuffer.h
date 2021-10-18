
#pragma once

#include "ModuleMinimal.h"
#include "RenderTarget.h"

class FRenderBuffer : public IRenderTarget
{

private: // Constructors

	explicit FRenderBuffer(uint16 InWidth, uint16 InHeight, ERenderTargetType InType);

public: // Destructors
	
	virtual ~FRenderBuffer();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FRenderBuffer> Create(uint16 Width, uint16 Height, ERenderTargetType Type)
	{ return MakeShareable(new FRenderBuffer(Width, Height, Type)); }
	
public: // IRenderTarget overrides

	FORCEINLINE virtual bool IsInitialized() const override { return Type != ERenderTargetType::Invalid; };
	FORCEINLINE virtual bool IsAttached() const override { return FBType != 0; };
	FORCEINLINE virtual ERenderTargetType GetType() const override { return Type; }

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