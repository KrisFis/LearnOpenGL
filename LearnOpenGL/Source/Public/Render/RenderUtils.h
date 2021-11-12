
#pragma once

#include "ModuleMinimal.h"

namespace NRenderUtils
{
	void Init();
	void Shutdown();

	void AllowUnbind(bool Value);
	bool IsUnbindAllowed();
	
	namespace NVertexArray
	{
		FVertexArrayId Generate();
		TArray<FVertexArrayId> Generate(const uint8 Num);
		
		void Delete(FVertexArrayId Id);
		void Delete(const TArray<FVertexArrayId>& Ids);
		
		void Bind(FVertexArrayId Id);
		void Unbind(bool Force = false);
		
		FVertexArrayId GetBound();
	}
	
	namespace NBuffer
	{
		FBufferId Generate();
		TArray<FBufferId> Generate(const uint8 Num);
		
		void Delete(FBufferId Id);
		void Delete(const TArray<FBufferId>& Ids);
		
		void Bind(EBufferTarget Target, FBufferId Id);
		void Unbind(EBufferTarget Target, bool Force = false);
		
		FBufferId GetBound(EBufferTarget Target);
	}
}