
#pragma once

#include "ModuleMinimal.h"

enum class ETextureType { JPEG, PNG };

class FTexture
{
public: // Constructors

	FTexture(const char* TextureFilename, const ETextureType TextureType);
	virtual ~FTexture();

public: // Getters

	inline bool IsInitialized() const { return bIsInitialized; }
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

	bool bIsInitialized;
};