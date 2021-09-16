
#pragma once

#include "ModuleMinimal.h"

struct FColor
{
	
public: // Static constructions

	// Construct color from RGB model
	inline static constexpr FColor RGB(uint8 Red, uint8 Green, uint8 Blue, uint8 Alpha) { return FColor(Red, Green, Blue, Alpha); }
	inline static constexpr FColor RGB(uint8 RGBA) { return FColor(RGBA, RGBA, RGBA, RGBA); }
	inline static constexpr FColor RGB(uint8 Red, uint8 Green, uint8 Blue) { return FColor(Red, Green, Blue, UINT8_MAX); }
	
	// Construct color from CMYK model 
	inline static constexpr FColor CMYK(float Cyan, float Magenta, float Yellow, float Black)
	{ 
		return FColor(
			(uint8)(UINT8_MAX * (1-Cyan) * (1-Black)),
			(uint8)(UINT8_MAX * (1-Magenta) * (1-Black)),
			(uint8)(UINT8_MAX * (1-Yellow) * (1-Black)),
			UINT8_MAX
		);
	}
		
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

	inline bool IsTransparent() const { return A == 0; } 

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

	constexpr FColor() : R(0), G(0), B(0), A(0) {}
	constexpr FColor(uint8 Red, uint8 Green, uint8 Blue, uint8 Alpha)
		: R(Red)
		, G(Green)
		, B(Blue)
		, A(Alpha)
	{}
};