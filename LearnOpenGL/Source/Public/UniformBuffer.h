
#pragma once

#include "ModuleMinimal.h"

class FUniformBuffer
{
	
private: // Constructors

	explicit FUniformBuffer(uint32 InPointIdx, uint32 InSize);

public: // Destructor

	~FUniformBuffer();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FUniformBuffer> Create(uint32 PointIdx, uint32 Size)
	{ return MakeShareable(new FUniformBuffer(PointIdx, Size)); }

public: // Getters

	FORCEINLINE bool IsInitialized() const { return bIsInitialized; }

	FORCEINLINE FBufferId GetId() const { return Id; }
	FORCEINLINE uint32 GetPointIdx() const { return PointIdx; }
	FORCEINLINE uint32 GetSize() const { return Size; }

public: // Setters

	template<typename T, typename TEnableIf<TIsPrimitiveType<T>::Value>::Type* = nullptr>
	FORCEINLINE void SetValue(uint32 Offset, const T& Value)
	{ FillSubData(Offset, sizeof(T), &Value); }

private: // Private methods

	void FillSubData(const uint32 Offset, const uint32 RealDataSize, const void* Data);

private: // Fields

	FBufferId Id;
	uint32 PointIdx;
	uint32 Size;

private: // Primitive fields

	bool bIsInitialized;

};

typedef TSharedPtr<FUniformBuffer> FUniformBufferPtr;