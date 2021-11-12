
#pragma once

#include "ModuleMinimal.h"
#include "RenderTargetBase.h"

class FRenderTexture : public IRenderTarget
{

private: // Constructors

	explicit FRenderTexture(uint8 InSamples, uint16 InWidth, uint16 InHeight, ERenderTargetType InType);

public: // Destructor
	
	virtual ~FRenderTexture();

public: // Static creation

	FORCEINLINE static TSharedPtr<FRenderTexture> Create(uint16 Width, uint16 Height, ERenderTargetType Type, uint8 Samples = 1)
	{ return MakeShareable(new FRenderTexture(Samples, Width, Height, Type)); }

public: // Getters

	FORCEINLINE FTextureId GetId() const { return Id; }

public: // IRenderTarget overrides

	FORCEINLINE virtual bool IsInitialized() const override { return Type != ERenderTargetType::Invalid; }
	FORCEINLINE virtual ERenderTargetType GetType() const override { return Type; }
	
	FORCEINLINE virtual EFramebufferType GetAttachedFBType() const override { return FBType; }
	FORCEINLINE virtual uint8 GetSamples() const override { return Samples; }

protected: // IRenderTarget overrides

	virtual bool AttachFramebuffer(const EFramebufferType FBTarget, const uint8 UseIndex) override;
	virtual bool DetachFramebuffer() override;
	
private: // Fields
	
	FTextureId Id;
	ERenderTargetType Type;
	EFramebufferType FBType;

	uint16 Width;
	uint16 Height;
	
	uint8 Samples;
};

typedef TSharedPtr<FRenderTexture> FRenderTexturePtr;