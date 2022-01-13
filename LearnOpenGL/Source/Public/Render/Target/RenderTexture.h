
#pragma once

#include "ModuleMinimal.h"
#include "RenderTargetBase.h"

struct ERenderTextureFlag
{
	enum
	{
		// Type
		UByte = 1 << 1,
		Float = 1 << 2,

		// Size
		Size8 = 1 << 3,
		Size16 = 1 << 4,
		Size32 = 1 << 5,

		// Num of channels
		R = 1 << 6,
		RG = 1 << 7,
		RGB = 1 << 8,
		RGBA = 1 << 9
	};
	
	static constexpr uint32 Default = UByte | Size8 | RGBA;
	static constexpr uint32 DefaultNoAlpha = UByte | Size8 | RGB;

	static constexpr uint32 DefaultFloat = Float | Size16 | RGBA;
	static constexpr uint32 DefaultFloatNoAlpha = Float | Size16 | RGB;
};

class FRenderTexture : public IRenderTarget
{

private: // Constructors

	explicit FRenderTexture(uint8 InSamples, uint16 InWidth, uint16 InHeight, ERenderTargetAttachmentType InType, uint32 InFlags);

public: // Destructor
	
	virtual ~FRenderTexture();

public: // Static creation

	FORCEINLINE static TSharedPtr<FRenderTexture> Create(uint16 Width, uint16 Height, ERenderTargetAttachmentType Type, uint32 Flags = ERenderTextureFlag::Default, uint8 Samples = 1)
	{ return MakeShareable(new FRenderTexture(Samples, Width, Height, Type, Flags)); }

public: // Getters

	FORCEINLINE FTextureId GetId() const { return Id; }

public: // IRenderTarget overrides

	FORCEINLINE virtual bool IsInitialized() const override { return Type != ERenderTargetAttachmentType::Invalid; }
	
	FORCEINLINE virtual ERenderTargetType GetType() const override { return ERenderTargetType::RenderTexture; }
	FORCEINLINE virtual ERenderTargetAttachmentType GetAttachmentType() const override { return Type; }
	FORCEINLINE virtual bool IsAttached() const override { return bIsAttached; }
	
	FORCEINLINE virtual uint8 GetSamples() const override { return Samples; }

protected: // IRenderTarget overrides

	virtual bool AttachFramebuffer(const uint8 UseIndex) override;
	virtual bool DetachFramebuffer() override;
	
private: // Fields
	
	FTextureId Id;
	ERenderTargetAttachmentType Type;

	uint16 Width;
	uint16 Height;
	
	uint8 Samples;

private: // Primitive fields

	uint8 bIsAttached : 1;
	
};

typedef TSharedPtr<FRenderTexture> FRenderTexturePtr;