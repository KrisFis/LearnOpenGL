
#pragma once

#include "IntegerTypes.h"

typedef uint32 FShaderId;
typedef uint32 FVertexArrayId;
typedef uint32 FBufferId;
typedef uint32 FFramebufferId;
typedef uint32 FTextureId;

typedef uint32 EBufferTarget;
typedef uint32 EFramebufferTarget;

namespace NRenderConsts
{
	namespace Invalid
	{
		constexpr FShaderId ShaderId = 0;
		constexpr FVertexArrayId VertexArrayId = 0;
		constexpr FBufferId BufferId = 0;
		constexpr FTextureId TextureId = 0;
	}
	
	namespace Default
	{
		constexpr FFramebufferId FramebufferId = 0;
	}
}

