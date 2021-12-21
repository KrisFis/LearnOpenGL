
#pragma once

#include "ModuleMinimal.h"

enum class ECubemapFace : uint8
{
	Right,
	Left,
	Top,
	Bottom,
	Front,
	Back
};

class FCubemap
{
	
private: // Constructors

	explicit FCubemap(const TFastMap<ECubemapFace, FString>& FacesPaths);

public: // Destructor

	virtual ~FCubemap();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FCubemap> Create(const TFastMap<ECubemapFace, FString>& FacesPaths)
	{ return MakeShareable(new FCubemap(FacesPaths)); }

public: // Getters

	FORCEINLINE FTextureId GetId() const { return Id; }
	FORCEINLINE int16 GetUsedIndex() const { return UsedIndex; }
	
	FORCEINLINE bool IsUsed() const { return UsedIndex != -1; }
	FORCEINLINE bool IsInitialized() const { return bIsInitialized; }

public: // Use methods

	void Use(const uint8 Index);
	void Clear();

private: // Fields

	FTextureId Id;
	
	int16 UsedIndex;

private: // Primitive fields

	uint8 bIsInitialized : 1;

};

typedef TSharedPtr<FCubemap> FCubemapPtr;