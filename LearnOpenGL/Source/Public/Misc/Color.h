
#pragma once

#include "ModuleMinimal.h"

struct FColor
{

public: // Constructors

	FORCEINLINE constexpr FColor()
		: R(0)
		, G(0)
		, B(0)
		, A(0)
	{}
	
	FORCEINLINE constexpr FColor(uint16 RGBA, uint16 Alpha = UINT8_MAX)
		: R(RGBA)
		, G(RGBA)
		, B(RGBA)
		, A(Alpha)
	{}
	
	FORCEINLINE constexpr FColor(uint16 Red, uint16 Green, uint16 Blue, uint16 Alpha = UINT8_MAX)
		: R(Red)
		, G(Green)
		, B(Blue)
		, A(Alpha)
	{}

public: // Static values
	
	static const FColor& GetEmpty();
	
public: // Conversions

	static FColor FromCMYK(uint8 Cyan, uint8 Magenta, uint8 Yellow, uint8 Black);
	static FColor FromHex(const char* Value);
	
	static FColor FromVec4(const glm::vec4& Value);
	static FColor FromVec3(const glm::vec3& Value);

	glm::vec4 ToVec4() const;
	glm::vec3 ToVec3() const;
	
public: // Checks

	FORCEINLINE bool IsNone() const { return R+G+B+A == 0; }
	FORCEINLINE bool IsTransparent() const { return A == 0; }
	
public: // Operations

	// Normalize color to be between 0 - 255
	void Normalize();
	FColor NormalizeCopy();
	
public: // Arithmetic operators (FColor)

	FColor& operator+=(const FColor& Other);
	FColor operator+(const FColor& Other) const;

	FColor& operator-=(const FColor& Other);
	FColor operator-(const FColor& Other) const;

	FColor& operator*=(const FColor& Other);
	FColor operator*(const FColor& Other) const;

	FColor& operator/=(const FColor& Other);
	FColor operator/(const FColor& Other) const;

public: // Arithmetic operators (uint16)

	FColor& operator+=(uint16 Value);
	FColor operator+(uint16 Value) const;

	FColor& operator-=(uint16 Value);
	FColor operator-(uint16 Value) const;

	FColor& operator*=(float Value);
	FColor operator*(float Value) const;

	FColor& operator/=(float Value);
	FColor operator/(float Value) const;

public: // Fields

	union { uint16 R, X; };
	union { uint16 G, Y; };
	union { uint16 B, Z; };
	union { uint16 A, W; };
};