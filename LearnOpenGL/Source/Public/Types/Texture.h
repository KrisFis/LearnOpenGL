
#pragma once

#include "ModuleMinimal.h"

enum class ETextureType : uint8
{
	Invalid = 0,
	Diffuse,
	Specular,
	Ambient,
	Emissive,
	Height,
	Normals,
	Shininess,
	Opacity,
	Displacement,
	LightMap,
	Reflection,
	BaseColor,
	NormalCamera,
	EmissionColor,
	Metalness,
	DiffuseRoughness,
	AmbientOcclusion
};

ETextureType ToTextureType(const aiTextureType Type);

class FTexture
{

private: // Constructors

	explicit FTexture(const char* InFilePath, const ETextureType InType, bool ClampToEdge);
	
public: // Destructors
	
	virtual ~FTexture();
	
public: // Static constructions

	FORCEINLINE static TSharedPtr<FTexture> Create(const char* FilePath, const ETextureType Type, bool ClampToEdge = false) 
	{ return MakeShareable(new FTexture(FilePath, Type, ClampToEdge));}

public: // Getters

	FORCEINLINE bool IsInitialized() const { return Type != ETextureType::Invalid; }
	FORCEINLINE bool IsUsed() const { return UseIndex != -1; }

	FORCEINLINE const FTextureId& GetId() const { return Id; }
	FORCEINLINE int16 GetUseIndex() const { return UseIndex; }
	
	FORCEINLINE ETextureType GetType() const { return Type; }
	FORCEINLINE const std::string& GetPath() const { return FilePath; }

public: // External methods

	void Use(const uint8 Index);
	void Clear();

private: // Fields

	FTextureId Id;
	int16 UseIndex;
	ETextureType Type;
	std::string FilePath;
};

typedef TSharedPtr<FTexture> FTexturePtr;