
#pragma once

#include "ModuleMinimal.h"

enum class ETextureType : uint8
{
	Invalid,
	Diffuse,
	Specular
};

class FTexture
{
public: // Constructors

	FTexture(const char* TextureFilename, const ETextureType TextureType);
	virtual ~FTexture();

public: // Getters

	inline bool IsInitialized() const { return Type != ETextureType::Invalid; }
	inline bool IsUsed() const { return UseIndex != -1; }

	inline const FTextureId& GetId() const { return Id; }
	inline int16 GetUseIndex() const { return UseIndex; }
	
	inline ETextureType GetType() const { return Type; }

public: // External methods

	void Use(const uint8 Index);
	void Clear();

private: // Fields

	FTextureId Id;
	int16 UseIndex;
	ETextureType Type;
};