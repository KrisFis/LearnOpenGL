
#pragma once

enum class ETextureType { JPEG, PNG };

class FTexture
{
public: // Constructors

	FTexture(const char* TextureFilename, const ETextureType TextureType);
	virtual ~FTexture();

public: // Getters

	inline bool IsInitialized() const { return bIsInitialized; }
	inline bool IsUsed() const { return UseIndex != -1; }

	inline const unsigned int& GetId() const { return Id; }
	inline short GetUseIndex() const { return UseIndex; }

	inline ETextureType GetType() const { return Type; }

public: // External methods

	void Use(unsigned char Index);
	void Clear();

private: // Fields

	unsigned int Id;
	short UseIndex;
	ETextureType Type;

	bool bIsInitialized;
};