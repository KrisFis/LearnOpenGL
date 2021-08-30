
#pragma once

#include "ModuleMinimal.h"

struct FColor
{

public: // Constructor

	constexpr FColor()
		: R(0)
		, G(0)
		, B(0)
		, A(0)
	{}

	constexpr FColor(uint8 RGBA)
		: R(RGBA)
		, G(RGBA)
		, B(RGBA)
		, A(RGBA)
	{}

	constexpr FColor(uint8 Red, uint8 Green, uint8 Blue)
		: R(Red)
		, G(Green)
		, B(Blue)
		, A(UINT8_MAX)
	{}

	constexpr FColor(uint8 Red, uint8 Green, uint8 Blue, uint8 Alpha)
		: R(Red)
		, G(Green)
		, B(Blue)
		, A(Alpha)
	{}

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

public: // Casts

	static FColor FromVec4(const glm::vec4& Value);
	static FColor FromVec3(const glm::vec3& Value);

	glm::vec4 ToVec4() const;
	glm::vec3 ToVec3() const;

public: //

	union { uint8 R, X; };
	union { uint8 G, Y; };
	union { uint8 B, Z; };
	union { uint8 A, W; };
};