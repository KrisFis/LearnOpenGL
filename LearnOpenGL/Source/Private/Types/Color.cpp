
#include "Color.h"

namespace NColorPrivate
{
	inline FColor& FillFrom(FColor& OutColor, const FColor& Other)
	{
		OutColor.R = Other.R;
		OutColor.G = Other.G;
		OutColor.B = Other.B;
		OutColor.A = Other.A;
		return OutColor;
	}
}

FColor FColor::operator+(const FColor& Other) const
{
	FColor newColor;
	newColor.R = (R > UINT8_MAX - Other.R) ? UINT8_MAX : R + Other.R;
	newColor.G = (G > UINT8_MAX - Other.G) ? UINT8_MAX : G + Other.G;
	newColor.B = (B > UINT8_MAX - Other.B) ? UINT8_MAX : B + Other.B;
	newColor.A = (A > UINT8_MAX - Other.A) ? UINT8_MAX : A + Other.A;
	return newColor;
}

FColor& FColor::operator+=(const FColor& Other)
{
	return NColorPrivate::FillFrom(*this, operator+(Other));
}

FColor FColor::operator-(const FColor& Other) const
{
	FColor newColor;
	newColor.R = (R < Other.R) ? 0 : R - Other.R;
	newColor.G = (G < Other.G) ? 0 : G - Other.G;
	newColor.B = (B < Other.B) ? 0 : B - Other.B;
	newColor.A = (A < Other.A) ? 0 : A - Other.A;
	return newColor;
}

FColor& FColor::operator-=(const FColor& Other)
{
	return NColorPrivate::FillFrom(*this, operator-(Other));
}

FColor FColor::operator*(const FColor& Other) const
{
	FColor newColor;
	newColor.R = (uint8)glm::clamp<float>((float)R * (float)Other.R / UINT8_MAX, 0.f, UINT8_MAX);
	newColor.G = (uint8)glm::clamp<float>((float)G * (float)Other.G / UINT8_MAX, 0.f, UINT8_MAX);
	newColor.B = (uint8)glm::clamp<float>((float)B * (float)Other.B / UINT8_MAX, 0.f, UINT8_MAX);
	newColor.A = (uint8)glm::clamp<float>((float)A * (float)Other.A / UINT8_MAX, 0.f, UINT8_MAX);
	return newColor;
}

FColor& FColor::operator*=(const FColor& Other)
{
	return NColorPrivate::FillFrom(*this, operator*(Other));
}

FColor FColor::operator/(const FColor& Other) const
{
	FColor newColor;
	newColor.R = (uint8)glm::clamp<float>((float)R / (float)Other.R / UINT8_MAX, 0.f, UINT8_MAX);
	newColor.G = (uint8)glm::clamp<float>((float)G / (float)Other.G / UINT8_MAX, 0.f, UINT8_MAX);
	newColor.B = (uint8)glm::clamp<float>((float)B / (float)Other.B / UINT8_MAX, 0.f, UINT8_MAX);
	newColor.A = (uint8)glm::clamp<float>((float)A / (float)Other.A / UINT8_MAX, 0.f, UINT8_MAX);
	return newColor;
}

FColor& FColor::operator/=(const FColor& Other)
{
	return NColorPrivate::FillFrom(*this, operator/(Other));
}

FColor FColor::operator+(uint8 Value) const
{
	return operator+(FColor(Value));
}

FColor& FColor::operator+=(uint8 Value)
{
	return operator+=(FColor(Value));
}

FColor FColor::operator-(uint8 Value) const
{
	return operator-(FColor(Value));
}

FColor& FColor::operator-=(uint8 Value)
{
	return operator-=(FColor(Value));
}

FColor FColor::operator*(float Value) const
{
	FColor newColor;
	newColor.R = (uint8)glm::clamp<float>((float)R * Value, 0.f, UINT8_MAX);
	newColor.G = (uint8)glm::clamp<float>((float)G * Value, 0.f, UINT8_MAX);
	newColor.B = (uint8)glm::clamp<float>((float)B * Value, 0.f, UINT8_MAX);
	newColor.A = (uint8)glm::clamp<float>((float)A * Value, 0.f, UINT8_MAX);
	return newColor;
}

FColor& FColor::operator*=(float Value)
{
	return NColorPrivate::FillFrom(*this, operator*(Value));
}

FColor FColor::operator/(float Value) const
{
	FColor newColor;
	newColor.R = (uint8)glm::clamp<float>((float)R / Value, 0.f, UINT8_MAX);
	newColor.G = (uint8)glm::clamp<float>((float)G / Value, 0.f, UINT8_MAX);
	newColor.B = (uint8)glm::clamp<float>((float)B / Value, 0.f, UINT8_MAX);
	newColor.A = (uint8)glm::clamp<float>((float)A / Value, 0.f, UINT8_MAX);
	return newColor;
}

FColor& FColor::operator/=(float Value)
{
	return NColorPrivate::FillFrom(*this, operator/(Value));
}

FColor FColor::FromVec4(const glm::vec4& Value)
{
	glm::vec4 normValue = glm::normalize(Value);
	normValue *= (float)UINT8_MAX;
	return { (uint8)normValue.x, (uint8)normValue.y, (uint8)normValue.z, (uint8)normValue.w };
}

FColor FColor::FromVec3(const glm::vec3& Value)
{
	glm::vec3 normValue = glm::normalize(Value);
	normValue *= (float)UINT8_MAX;
	return { (uint8)normValue.x, (uint8)normValue.y, (uint8)normValue.z };
}

glm::vec4 FColor::ToVec4() const
{
	return {(float)R / UINT8_MAX, (float)G / UINT8_MAX, (float)B / UINT8_MAX, (float)A / UINT8_MAX };
}

glm::vec3 FColor::ToVec3() const
{
	return {(float)R / UINT8_MAX, (float)G / UINT8_MAX, (float)B / UINT8_MAX };
}
