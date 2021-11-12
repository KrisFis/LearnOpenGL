
#include "ShaderUtils.h"

namespace NShaderUtils
{
	constexpr uint8 ElementSize = (uint8)sizeof(GLfloat);

	namespace NPrivate
	{
		uint32 CorrectUniformSizeType_Impl(uint32 DataSize)
		{
			if(DataSize <= ElementSize) return ElementSize;
			
			// iterate by 2N
			constexpr uint8 iterator = 2 * ElementSize;
			// how many times iterator fits base
			const double fitsIterator = (double)DataSize / iterator;
			// ceil to upper int
			const uint32 fitsIteratorCeiled = (uint32)glm::ceil(fitsIterator);
			// return back iterator to data size
			return fitsIteratorCeiled * iterator;
		}
	}

	bool IsSTD140Size(uint32 DataSize)
	{
		if(DataSize == ElementSize) return true;
		
		// iterate by 2
		constexpr uint8 iterator = 2;
		// does data size is dividable without rest
		return (DataSize % iterator == 0);
	}
}