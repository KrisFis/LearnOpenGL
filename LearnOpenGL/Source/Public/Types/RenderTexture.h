
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

	inline virtual bool IsInitialized() const override { return Type != ERenderTargetType::Invalid; }
	inline virtual bool IsAttached() const override { return Target != 0; }
	inline virtual ERenderTargetType GetType() const override { return Type; }

protected: // IRenderTarget overrides

	virtual bool AttachFramebuffer(const EFramebufferTarget FBTarget) override;
	virtual bool DetachFramebuffer() override;
	
private: // Fields
	
	FTextureId Id;
	ERenderTargetType Type;
	EFramebufferTarget Target;

	uint16 Width;
	uint16 Height;
};