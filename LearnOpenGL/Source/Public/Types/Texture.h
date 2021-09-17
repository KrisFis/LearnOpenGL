
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

public: // Constructors

	FTexture(const char* InFilePath, const ETextureType InType, bool ClampToEdge = false);
	virtual ~FTexture();
	
public: // Static constructions

	static FTexture CreateRenderTexture(uint16 Width, uint16 Height, bool ForDepth, bool ForStencil);

public: // Getters

	inline bool IsInitialized() const { return Type != ETextureType::Invalid; }
	inline bool IsUsed() const { return UseIndex != -1; }

	inline const FTextureId& GetId() const { return Id; }
	inline int16 GetUseIndex() const { return UseIndex; }
	
	inline ETextureType GetType() const { return Type; }
	inline const std::string& GetPath() const { return FilePath; }

public: // External methods

	void Use(const uint8 Index);
	void Clear();

private: // Fields

	FTextureId Id;
	int16 UseIndex;
	ETextureType Type;
	std::string FilePath;
};