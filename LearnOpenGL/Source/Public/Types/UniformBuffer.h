
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

	FORCEINLINE FBufferId GetId() const { return Id; }
	FORCEINLINE uint32 GetPointIdx() const { return PointIdx; }
	FORCEINLINE uint32 GetSize() const { return Size; }

public: // Setters

	template<typename T>
	FORCEINLINE void SetValue(uint32 Offset, const T& Value)
	{ FillSubData(Offset, sizeof(T), &Value); }

private: // Private methods

	void FillSubData(uint32 Offset, uint32 DataSize, const void* Data);

private: // Fields

	FBufferId Id;
	uint32 PointIdx;
	uint32 Size;
};

typedef TSharedPtr<FUniformBuffer> FUniformBufferPtr;