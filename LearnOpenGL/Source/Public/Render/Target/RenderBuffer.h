
#pragma once

#include "ModuleMinimal.h"
#include "RenderTargetBase.h"

class FRenderBuffer : public IRenderTarget
{

private: // Constructors

	explicit FRenderBuffer(uint8 InSamples, uint16 InWidth, uint16 InHeight, ERenderTargetAttachmentType InType);

public: // Destructors
	
	virtual ~FRenderBuffer();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FRenderBuffer> Create(uint16 Width, uint16 Height, ERenderTargetAttachmentType Type, uint8 Samples = 1)
	{ return MakeShareable(new FRenderBuffer(Samples, Width, Height, Type)); }
	
public: // IRenderTarget overrides

	FORCEINLINE virtual bool IsInitialized() const override { return Type != ERenderTargetAttachmentType::Invalid; };
	
	FORCEINLINE virtual ERenderTargetType GetType() const override { return ERenderTargetType::RenderBuffer; }
	FORCEINLINE virtual ERenderTargetAttachmentType GetAttachmentType() const override { return Type; }
	
	FORCEINLINE virtual bool IsAttached() const override { return bIsAttached; }
	FORCEINLINE virtual uint8 GetSamples() const override { return Samples; }

protected: // IRenderTarget overrides

	virtual bool AttachFramebuffer(const uint8 UseIndex) override;
	virtual bool DetachFramebuffer() override;

private: // Fields

	FRenderBufferId Id;
	ERenderTargetAttachmentType Type;

	uint16 Width;
	uint16 Height;
	
	uint8 Samples;

private: // Primitive Fields

	uint8 bIsAttached : 1;

};