
#pragma once

#include "AlternityShared.h"
#include "IntegerTypes.h"

typedef uint32 FShaderProgramId;
typedef uint32 FVertexArrayId;
typedef uint32 FBufferId;
typedef uint32 FFramebufferId;
typedef uint32 FRenderBufferId;
typedef uint32 FTextureId;

typedef uint32 EBufferTarget;
typedef uint32 EFramebufferType;

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

typedef TSharedPtr<class IRenderTarget> FRenderTargetPtr;
typedef TSharedPtr<class FRenderTexture> FRenderTexturePtr;
typedef TSharedPtr<class FRenderBuffer> FRenderBufferPtr;