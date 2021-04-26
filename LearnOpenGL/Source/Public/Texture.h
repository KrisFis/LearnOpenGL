
#pragma once

enum class ETextureType { JPEG, PNG };

class FTexture
{
public: // Constructors

	FTexture(const char* TextureFilename, const ETextureType TextureType);
	virtual ~FTexture();

public: // Getters

	inline bool IsInitialized() const { return bIsInitialized; }

	inline const unsigned int& GetId() const { return Id; }
	inline ETextureType GetType() const { return Type; }

public: // External methods

	void Apply();

private: // Fields

	unsigned int Id;

	ETextureType Type;

	bool bIsInitialized;

};