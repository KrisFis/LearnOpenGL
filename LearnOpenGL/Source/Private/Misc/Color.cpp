
#include "Color.h"
#include "StringUtils.h"

const FColor& FColor::GetEmpty()
{
	static FColor emptyVal = FColor();
	return emptyVal;
}

FColor FColor::FromCMYK(uint8 Cyan, uint8 Magenta, uint8 Yellow, uint8 Black)
{
	return FColor(
		(UINT8_MAX - Cyan) * (UINT8_MAX - Black),
		(UINT8_MAX - Magenta) * (UINT8_MAX - Black),
		(UINT8_MAX - Yellow) * (UINT8_MAX - Black),
		UINT8_MAX
	);
}

FColor FColor::FromHex(const char* Value)
{
	TArray<uint8> bytes = NStringUtils::HexToBytes(Value);
	const uint16 bytesNum = (uint16)bytes.size();
	
	return FColor(
		(bytesNum > 0) ? bytes[0] : 0,
		(bytesNum > 1) ? bytes[1] : 0,
		(bytesNum > 2) ? bytes[2] : 0,
		(bytesNum > 3) ? bytes[3] : UINT8_MAX
	); 
}

FColor FColor::FromVec4(const glm::vec4& Value)
{
	return {(uint16)Value.x, (uint16)Value.y, (uint16)Value.z, (uint16)Value.w };
}

FColor FColor::FromVec3(const glm::vec3& Value)
{
	return {(uint16)Value.x, (uint16)Value.y, (uint16)Value.z, UINT8_MAX };
}

glm::vec4 FColor::ToVec4() const
{
	return {(float)R / UINT8_MAX, (float)G / UINT8_MAX, (float)B / UINT8_MAX, (float)A / UINT8_MAX };
}

glm::vec3 FColor::ToVec3() const
{
	return {(float)R / UINT8_MAX, (float)G / UINT8_MAX, (float)B / UINT8_MAX };
}

void FColor::Normalize()
{
	R = glm::clamp<uint16>(R, 0, UINT8_MAX);
	G = glm::clamp<uint16>(G, 0, UINT8_MAX);
	B = glm::clamp<uint16>(B, 0, UINT8_MAX);
	A = glm::clamp<uint16>(A, 0, UINT8_MAX);
}

FColor FColor::NormalizeCopy()
{
	FColor newColor(*this);
	newColor.Normalize();
	return newColor;
}


FColor& FColor::operator+=(const FColor& Other)
{
#if BUILD_DEBUG
	ENSURE(R <= UINT16_MAX - Other.R);
	ENSURE(G <= UINT16_MAX - Other.G);
	ENSURE(B <= UINT16_MAX - Other.B);
	ENSURE(A <= UINT16_MAX - Other.A);
#endif // BUILD_DEBUG
	
	R += Other.R;
	G += Other.G;
	B += Other.B;
	A += Other.A;
	return *this;
}

FColor FColor::operator+(const FColor& Other) const
{
	FColor newColor(*this);
	newColor += Other;
	return newColor;
}

FColor& FColor::operator-=(const FColor& Other)
{
#if BUILD_DEBUG
	ENSURE(R > Other.R);
	ENSURE(G > Other.G);
	ENSURE(B > Other.B);
	ENSURE(A > Other.A);
#endif // BUILD_DEBUG

	R -= Other.R;
	G -= Other.G;
	B -= Other.B;
	A -= Other.A;
	return *this;
}

FColor FColor::operator-(const FColor& Other) const
{
	FColor newColor(*this);
	newColor -= Other;
	return newColor;
}

FColor& FColor::operator*=(const FColor& Other)
{
#if BUILD_DEBUG
	ENSURE(R <= UINT16_MAX / Other.R);
	ENSURE(G <= UINT16_MAX / Other.G);
	ENSURE(B <= UINT16_MAX / Other.B);
	ENSURE(A <= UINT16_MAX / Other.A);
#endif // BUILD_DEBUG

	R *= Other.R;
	G *= Other.G;
	B *= Other.B;
	A *= Other.A;
	return *this;
}

FColor FColor::operator*(const FColor& Other) const
{
	FColor newColor(*this);
	newColor *= Other;
	return newColor;
}

FColor& FColor::operator/=(const FColor& Other)
{
#if BUILD_DEBUG
	ENSURE(Other.R > 0);
	ENSURE(Other.G > 0);
	ENSURE(Other.B > 0);
	ENSURE(Other.A > 0);
#endif // BUILD_DEBUG

	R /= Other.R;
	G /= Other.G;
	B /= Other.B;
	A /= Other.A;
	return *this;
}

FColor FColor::operator/(const FColor& Other) const
{
	FColor newColor(*this);
	newColor /= Other;
	return newColor;
}

FColor& FColor::operator+=(uint16 Value)
{
	return operator+=(FColor(Value,Value,Value,Value));
}

FColor FColor::operator+(uint16 Value) const
{
	return operator+(FColor(Value,Value,Value,Value));
}

FColor& FColor::operator-=(uint16 Value)
{
	return operator-=(FColor(Value,Value,Value,Value));
}

FColor FColor::operator-(uint16 Value) const
{
	return operator-(FColor(Value,Value,Value,Value));
}

FColor& FColor::operator*=(float Value)
{
#if BUILD_DEBUG
	ENSURE((float)R <= (float)UINT16_MAX / Value);
	ENSURE((float)G <= (float)UINT16_MAX / Value);
	ENSURE((float)B <= (float)UINT16_MAX / Value);
	ENSURE((float)A <= (float)UINT16_MAX / Value);
#endif // BUILD_DEBUG

	R = (uint16)(R * Value);
	G = (uint16)(G * Value);
	B = (uint16)(B * Value);
	A = (uint16)(A * Value);
	return *this;
}

FColor FColor::operator*(float Value) const
{
	FColor newColor(*this);
	newColor *= Value;
	return newColor;
}

FColor& FColor::operator/=(float Value)
{
#if BUILD_DEBUG
	ENSURE(Value > 0);
#endif // BUILD_DEBUG

	R = (uint16)(R / Value);
	G = (uint16)(G / Value);
	B = (uint16)(B / Value);
	A = (uint16)(A / Value);
	return *this;
}

FColor FColor::operator/(float Value) const
{
	FColor newColor(*this);
	newColor /= Value;
	return newColor;
}