
#pragma once

#include "ModuleMinimal.h"

class FTexture
{
public: // Constructors

	FTexture(const char* TextureFilename);
	virtual ~FTexture();

public: // Getters

	inline bool IsInitialized() const { return bIsInitialized; }
	inline bool IsUsed() const { return UseIndex != -1; }

	inline const FTextureId& GetId() const { return Id; }
	inline int16 GetUseIndex() const { return UseIndex; }

public: // External methods

	void Use(const uint8 Index);
	void Clear();

private: // Fields

	FTextureId Id;
	int16 UseIndex;

	bool bIsInitialized;
};