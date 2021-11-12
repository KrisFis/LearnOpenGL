
#pragma once

#include "ModuleMinimal.h"

namespace NShaderUtils
{
	namespace NPrivate
	{
		template<typename T> uint32 GetSTD140SizeType_Impl();
		DECLARE_VARIADIC_ITERATOR_RET_TRAIT(FSTD140SizeHelper, uint32, +, GetSTD140SizeType_Impl);
		
		uint32 CorrectUniformSizeType_Impl(uint32 DataSize);
	}
	
	// Does quick check whether size is really correct
	// * Can still be invalid if called badly calculated 
	bool IsSTD140Size(uint32 DataSize);
	
	// Calculates correct uniform type size
	// * Should be used for primitive type
	template<typename... ArgTypes>
	uint32 GetSTD140Size();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// TEMPLATE IMPLEMENTATIONS
/////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> 
FORCEINLINE uint32 NShaderUtils::NPrivate::GetSTD140SizeType_Impl()
{ 
	return CorrectUniformSizeType_Impl(sizeof(T)); 
}

template<typename... ArgTypes>
FORCEINLINE uint32 NShaderUtils::GetSTD140Size()
{ 
	return NPrivate::EXECUTE_VARIADIC_ITERATOR_TRAIT(FSTD140SizeHelper, ArgTypes); 
}

