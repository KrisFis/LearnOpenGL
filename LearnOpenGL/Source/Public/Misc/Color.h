
#pragma once

#include "ModuleMinimal.h"

struct FColor
{
public: // Default constructor

	FORCEINLINE constexpr FColor() : R(0), G(0), B(0), A(0) {}

private: // Internal constructions

	template<typename ParType, typename TEnableIf<!TIsFloatingType<ParType>::Value>::Type* = nullptr>
	FORCEINLINE static constexpr FColor ConstructImpl(ParType Red, ParType Green, ParType Blue, ParType Alpha)
	{
		return FColor((uint8)Red, (uint8)Green, (uint8)Blue, (uint8)Alpha);
	}
	
	template<typename ParType, typename TEnableIf<TIsFloatingType<ParType>::Value>::Type* = nullptr>
	FORCEINLINE static constexpr FColor ConstructImpl(ParType Red, ParType Green, ParType Blue, ParType Alpha)
	{
		return FColor(
			(uint8)(glm::clamp(Red, 0.f, 1.f) * (float)UINT8_MAX),
			(uint8)(glm::clamp(Green, 0.f, 1.f) * (float)UINT8_MAX),
			(uint8)(glm::clamp(Blue, 0.f, 1.f) * (float)UINT8_MAX),
			(uint8)(glm::clamp(Alpha, 0.f, 1.f) * (float)UINT8_MAX)
		);
	}
	
public: // Static constructions
	
	// Construct color from RGB model
	template<typename ParType>
	FORCEINLINE static constexpr FColor RGB(ParType RGBA) { return ConstructImpl(RGBA, RGBA, RGBA, RGBA); }
	template<typename ParType>
	FORCEINLINE static constexpr FColor RGB(ParType Red, ParType Green, ParType Blue) { return ConstructImpl(Red, Green, Blue, (ParType)UINT8_MAX); }
	template<typename ParType>
	FORCEINLINE static constexpr FColor RGB(ParType Red, ParType Green, ParType Blue, ParType Alpha) { return ConstructImpl(Red, Green, Blue, Alpha); }
	
	// Construct color from CMYK model 
	FORCEINLINE static constexpr FColor CMYK(float Cyan, float Magenta, float Yellow, float Black)
	{ return ConstructImpl((1-Cyan) * (1-Black), (1-Magenta) * (1-Black), (1-Yellow) * (1-Black), (float)UINT8_MAX); }
		
public: // Arithmetic operators (FColor)

	FColor operator+(const FColor& Other) const;
	FColor& operator+=(const FColor& Other);

	FColor operator-(const FColor& Other) const;
	FColor& operator-=(const FColor& Other);

	FColor operator*(const FColor& Other) const;
	FColor& operator*=(const FColor& Other);

	FColor operator/(const FColor& Other) const;
	FColor& operator/=(const FColor& Other);

public: // Arithmetic operators (uint8)

	FColor operator+(uint8 Value) const;
	FColor& operator+=(uint8 Value);

	FColor operator-(uint8 Value) const;
	FColor& operator-=(uint8 Value);

	FColor operator*(float Value) const;
	FColor& operator*=(float Value);

	FColor operator/(float Value) const;
	FColor& operator/=(float Value);

public: // Checks

	FORCEINLINE bool IsTransparent() const { return A == 0; } 

public: // Casts

	static FColor FromHex(const char* Value);
	static FColor FromVec4(const glm::vec4& Value);
	static FColor FromVec3(const glm::vec3& Value);
	
	glm::vec4 ToVec4() const;
	glm::vec3 ToVec3() const;

public: // Fields

	union { uint8 R, X; };
	union { uint8 G, Y; };
	union { uint8 B, Z; };
	union { uint8 A, W; };

private: // Private constructor

	FORCEINLINE constexpr FColor(uint8 Red, uint8 Green, uint8 Blue, uint8 Alpha)
		: R(Red)
		, G(Green)
		, B(Blue)
		, A(Alpha)
	{}
};