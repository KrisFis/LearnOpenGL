
#pragma once

#include <memory>
#include "IntegerTypes.h"

typedef uint32 FShaderProgramId;
typedef uint32 FVertexArrayId;
typedef uint32 FBufferId;
typedef uint32 FFramebufferId;
typedef uint32 FRenderBufferId;
typedef uint32 FTextureId;

typedef uint32 EBufferTarget;
typedef uint32 EFramebufferTarget;

namespace NRenderConsts
{
	namespace Invalid
	{
		constexpr FShaderProgramId ShaderProgramId = 0;
		constexpr FVertexArrayId VertexArrayId = 0;
		constexpr FBufferId BufferId = 0;
		constexpr FTextureId TextureId = 0;
	}
	
	namespace Default
	{
		constexpr FFramebufferId FramebufferId = 0;
	}
}

typedef std::shared_ptr<class FRenderTexture> FRenderTexturePtr;
typedef std::shared_ptr<class FRenderBuffer> FRenderBufferPtr;