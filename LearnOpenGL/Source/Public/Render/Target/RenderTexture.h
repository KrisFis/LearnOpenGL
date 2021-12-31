
#pragma once

#include "ModuleMinimal.h"
#include "RenderTargetBase.h"

struct FRenderTextureFlag
{
	// Color flags
	enum EColor
	{
		// Default is size 8 and type unsigned byte
		
		// Includes alpha
		WithAlpha = 0x01,
		
		// Size
		Float16 = 0x02,
		Float32 = 0x04
	};
	
	static constexpr uint8 None = 0;
};

typedef FRenderTextureFlag::EColor ERenderTextureColorFlag;

class FRenderTexture : public IRenderTarget
{

private: // Constructors

	explicit FRenderTexture(uint8 InSamples, uint16 InWidth, uint16 InHeight, ERenderTargetType InType, uint8 InFlags);

public: // Destructor
	
	virtual ~FRenderTexture();

public: // Static creation

	FORCEINLINE static TSharedPtr<FRenderTexture> Create(uint16 Width, uint16 Height, ERenderTargetType Type, uint8 Flags = FRenderTextureFlag::None, uint8 Samples = 1)
	{ return MakeShareable(new FRenderTexture(Samples, Width, Height, Type, Flags)); }

public: // Getters

	FORCEINLINE FTextureId GetId() const { return Id; }

public: // IRenderTarget overrides

	FORCEINLINE virtual bool IsInitialized() const override { return Type != ERenderTargetType::Invalid; }
	FORCEINLINE virtual ERenderTargetType GetType() const override { return Type; }
	FORCEINLINE virtual bool IsAttached() const override { return bIsAttached; }
	
	FORCEINLINE virtual uint8 GetSamples() const override { return Samples; }

protected: // IRenderTarget overrides

	virtual bool AttachFramebuffer(const uint8 UseIndex) override;
	virtual bool DetachFramebuffer() override;
	
private: // Fields
	
	FTextureId Id;
	ERenderTargetType Type;

	uint16 Width;
	uint16 Height;
	
	uint8 Samples;

private: // Primitive fields

	uint8 bIsAttached : 1;
	
};

typedef TSharedPtr<FRenderTexture> FRenderTexturePtr;