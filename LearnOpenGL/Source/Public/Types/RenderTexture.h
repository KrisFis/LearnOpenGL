
#pragma once

#include "ModuleMinimal.h"
#include "RenderTarget.h"

class FRenderTexture : public IRenderTarget
{
	
public: // Constructors

	FRenderTexture(uint16 InWidth, uint16 InHeight, ERenderTargetType InType);
	virtual ~FRenderTexture();

	void AssignFramebuffer();

public: // IRenderTarget overrides

	FORCEINLINE virtual bool IsInitialized() const override { return Type != ERenderTargetType::Invalid; }
	FORCEINLINE virtual bool IsAttached() const override { return FBType != 0; }
	FORCEINLINE virtual ERenderTargetType GetType() const override { return Type; }

protected: // IRenderTarget overrides

	virtual bool AttachFramebuffer(const EFramebufferType FBTarget) override;
	virtual bool DetachFramebuffer() override;
	
private: // Fields
	
	FTextureId Id;
	ERenderTargetType Type;
	EFramebufferType FBType;

	uint16 Width;
	uint16 Height;
};