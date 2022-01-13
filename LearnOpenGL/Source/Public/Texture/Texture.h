
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

	explicit FTexture(const FRenderTexture* RenderTexture, const ETextureType InType);
	explicit FTexture(const char* InFilePath, const ETextureType InType, bool IsLinear, bool ClampToEdge);
	explicit FTexture(const TArray<glm::vec4>& InData, const ETextureType InType, bool IsLinear, bool ClampToEdge); // Make more options or create factory
	
public: // Destructors
	
	virtual ~FTexture();
	
public: // Static constructions

	FORCEINLINE static TSharedPtr<FTexture> Create(const FRenderTexture* RenderTexture, const ETextureType Type) 
	{ return MakeShareable(new FTexture(RenderTexture, Type));}

	FORCEINLINE static TSharedPtr<FTexture> Create(const char* FilePath, const ETextureType Type, bool IsLinear = false, bool ClampToEdge = false) 
	{ return MakeShareable(new FTexture(FilePath, Type, IsLinear, ClampToEdge));}

	FORCEINLINE static TSharedPtr<FTexture> Create(const TArray<glm::vec4>& Data, const ETextureType Type, bool IsLinear = false, bool ClampToEdge = false)
	{ return MakeShareable(new FTexture(Data, Type, IsLinear, ClampToEdge)); }

public: // Getters

	FORCEINLINE bool IsInitialized() const { return Type != ETextureType::Invalid; }
	FORCEINLINE bool IsUsed() const { return UseIndex != -1; }

	FORCEINLINE const FTextureId& GetId() const { return Id; }
	FORCEINLINE int16 GetUseIndex() const { return UseIndex; }
	
	FORCEINLINE ETextureType GetType() const { return Type; }
	FORCEINLINE const FString& GetPath() const { return FilePath; }

public: // Use

	void Use(const uint8 Index);
	void Clear();

private: // Fields

	FTextureId Id;
	int16 UseIndex;
	ETextureType Type;
	FString FilePath;

private: // Primitive Fields

	uint8 bTargetInit : 1;

};

typedef TSharedPtr<FTexture> FTexturePtr;