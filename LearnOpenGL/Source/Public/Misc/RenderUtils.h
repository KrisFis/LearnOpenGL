
#pragma once

#include "ModuleMinimal.h"

namespace NRenderUtils
{
	void Init();
	void Shutdown();

	void AllowUnbind(bool Value);
	bool IsUnbindAllowed();

	FVertexArrayId GenerateVertexArray();
	FBufferId GenerateBuffer();
	// TODO(JKF): FShaderId GenerateShader();
	// TODO(JKF): FTextureId GenerateTexture();

	std::vector<FVertexArrayId> GenerateVertexArrays(const uint8 Num);
	std::vector<FBufferId> GenerateBuffers(const uint8 Num);
	// TODO(JKF): std::vector<FShaderId> GenerateShaders(const uint8 Num);
	// TODO(JKF): std::vector<FTextureId> GenerateTextures(const uint8 Num);

	void DeleteVertexArray(FBufferId Id);
	void DeleteBuffer(FBufferId Id);
	void DeleteFramebuffer(FFramebufferId Id);

	void DeleteVertexArrays(const std::vector<FVertexArrayId>& Ids);
	void DeleteBuffers(const std::vector<FBufferId>& Ids);

	void BindVertexArray(FVertexArrayId Id);
	void BindBuffer(EBufferTarget Target, FBufferId Id);

	void UnbindVertexArray(bool Force = false);
	void UnbindBuffer(EBufferTarget Target, bool Force = false);

	FVertexArrayId GetBoundVertexArray();
	FBufferId GetBoundBuffer(EBufferTarget Target);
}