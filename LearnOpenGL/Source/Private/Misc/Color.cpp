
#include "Color.h"
#include "StringUtils.h"

namespace NColorPrivate
{
	FORCEINLINE void CheckAdd(const FColor& A, const FColor& B)
	{
		#if BUILD_DEBUG
		ENSURE(A.R <= UINT16_MAX - B.R);
		ENSURE(A.G <= UINT16_MAX - B.G);
		ENSURE(A.B <= UINT16_MAX - B.B);
		ENSURE(A.A <= UINT16_MAX - B.A);
		#endif // BUILD_DEBUG
	}
	
	FORCEINLINE void CheckSubstract(const FColor& A, const FColor& B)
	{
		#if BUILD_DEBUG
		ENSURE(A.R < B.R);
		ENSURE(A.G < B.G);
		ENSURE(A.B < B.B);
		ENSURE(A.A < B.A);
		#endif // BUILD_DEBUG
	}
	
	FORCEINLINE void CheckMultiply(const FColor& A, const FColor& B)
	{
		#if BUILD_DEBUG
		ENSURE(A.R > UINT16_MAX / B.R);
		ENSURE(A.G > UINT16_MAX / B.G);
		ENSURE(A.B > UINT16_MAX / B.B);
		ENSURE(A.A > UINT16_MAX / B.A);
		#endif // BUILD_DEBUG
	}
	
	FORCEINLINE void CheckMultiply(const FColor& A, float Value)
	{
		#if BUILD_DEBUG
		ENSURE(A.R > UINT16_MAX / Value);
		ENSURE(A.G > UINT16_MAX / Value);
		ENSURE(A.B > UINT16_MAX / Value);
		ENSURE(A.A > UINT16_MAX / Value);
		#endif // BUILD_DEBUG
	}
	
	FORCEINLINE void CheckDivide(const FColor& A, const FColor& B)
	{
		#if BUILD_DEBUG
		ENSURE(B.R > 0);
		ENSURE(B.G > 0);
		ENSURE(B.B > 0);
		ENSURE(B.A > 0);
		#endif // BUILD_DEBUG
	}
	
	FORCEINLINE void CheckDivide(const FColor& A, float Value)
	{
		#if BUILD_DEBUG
		ENSURE(Value > 0);
		#endif // BUILD_DEBUG
	}
}

const FColor& FColor::GetEmpty()
{
	static FColor emptyVal = FColor();
	return emptyVal;
}

FColor FColor::FromCMYK(float Cyan, float Magenta, float Yellow, float Black)
{
	return FColor((1-Cyan) * (1-Black), (1-Magenta) * (1-Black), (1-Yellow) * (1-Black), (float)UINT8_MAX);
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

void FColor::Normalize(FColor& OutColor)
{
	OutColor.R = glm::clamp<uint16>(R, 0, UINT8_MAX);
	OutColor.G = glm::clamp<uint16>(G, 0, UINT8_MAX);
	OutColor.B = glm::clamp<uint16>(B, 0, UINT8_MAX);
	OutColor.A = glm::clamp<uint16>(A, 0, UINT8_MAX);
}

FColor FColor::operator+(const FColor& Other) const
{
	NColorPrivate::CheckAdd(*this, Other);
	
	FColor newColor;
	newColor.R = R + Other.R;
	newColor.G = G + Other.G;
	newColor.B = B + Other.B;
	newColor.A = A + Other.A;
	return newColor;
}

FColor& FColor::operator+=(const FColor& Other)
{
	NColorPrivate::CheckAdd(*this, Other);
	
	R += Other.R;
	G += Other.G;
	B += Other.B;
	A += Other.A;
	return *this;
}

FColor FColor::operator-(const FColor& Other) const
{
	NColorPrivate::CheckSubstract(*this, Other);

	FColor newColor;
	newColor.R = R - Other.R;
	newColor.G = G - Other.G;
	newColor.B = B - Other.B;
	newColor.A = A - Other.A;
	return newColor;
}

FColor& FColor::operator-=(const FColor& Other)
{
	NColorPrivate::CheckSubstract(*this, Other);

	R -= Other.R;
	G -= Other.G;
	B -= Other.B;
	A -= Other.A;
	return *this;
}

FColor FColor::operator*(const FColor& Other) const
{
	NColorPrivate::CheckMultiply(*this, Other);

	FColor newColor;
	newColor.R = R * Other.R;
	newColor.G = G * Other.G;
	newColor.B = B * Other.B;
	newColor.A = A * Other.A;
	return newColor;
}

FColor& FColor::operator*=(const FColor& Other)
{
	NColorPrivate::CheckMultiply(*this, Other);

	R *= Other.R;
	G *= Other.G;
	B *= Other.B;
	A *= Other.A;
	return *this;
}

FColor FColor::operator/(const FColor& Other) const
{
	NColorPrivate::CheckDivide(*this, Other);

	FColor newColor;
	newColor.R = R / Other.R;
	newColor.G = G / Other.G;
	newColor.B = B / Other.B;
	newColor.A = A / Other.A;
	return newColor;
}

FColor& FColor::operator/=(const FColor& Other)
{
	NColorPrivate::CheckDivide(*this, Other);

	R /= Other.R;
	G /= Other.G;
	B /= Other.B;
	A /= Other.A;
	return *this;
}

FColor FColor::operator+(uint16 Value) const
{
	return operator+(FColor(Value,Value,Value,Value));
}

FColor& FColor::operator+=(uint16 Value)
{
	return operator+=(FColor(Value,Value,Value,Value));
}

FColor FColor::operator-(uint16 Value) const
{
	return operator-(FColor(Value,Value,Value,Value));
}

FColor& FColor::operator-=(uint16 Value)
{
	return operator-=(FColor(Value,Value,Value,Value));
}

FColor FColor::operator*(float Value) const
{
	NColorPrivate::CheckMultiply(*this, Value);

	FColor newColor;
	newColor.R = R * Value;
	newColor.G = G * Value;
	newColor.B = B * Value;
	newColor.A = A * Value;
	return newColor;
}

FColor& FColor::operator*=(float Value)
{
	NColorPrivate::CheckMultiply(*this, Value);

	R *= Value;
	G *= Value;
	B *= Value;
	A *= Value;
	return *this;
}

FColor FColor::operator/(float Value) const
{
	NColorPrivate::CheckDivide(*this, Value);

	FColor newColor;
	newColor.R = R / Value;
	newColor.G = G / Value;
	newColor.B = B / Value;
	newColor.A = A / Value;
	return newColor;
}

FColor& FColor::operator/=(float Value)
{
	NColorPrivate::CheckDivide(*this, Value);

	R /= Value;
	G /= Value;
	B /= Value;
	A /= Value;
	return *this;
}