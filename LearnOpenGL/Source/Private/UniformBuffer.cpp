
#include "UniformBuffer.h"
#include "RenderUtils.h"
#include "ShaderUtils.h"

FUniformBuffer::FUniformBuffer(uint32 InPointIdx, uint32 InSize)
	: Id(0)
	, PointIdx(InPointIdx)
	, Size(InSize)
	, bIsInitialized(false)
{
	ENSURE_RET(NShaderUtils::IsSTD140Size(Size));

	Id = NRenderUtils::NBuffer::Generate();
	NRenderUtils::NBuffer::Bind(GL_UNIFORM_BUFFER, Id);
	
	glBufferData(GL_UNIFORM_BUFFER, Size, nullptr, GL_STATIC_DRAW);
	
	NRenderUtils::NBuffer::Unbind(GL_UNIFORM_BUFFER);
	
	glBindBufferRange(GL_UNIFORM_BUFFER, PointIdx, Id, 0, Size);
	
	bIsInitialized = true;
}

FUniformBuffer::~FUniformBuffer()
{
	if(Id != NRenderConsts::Invalid::BufferId)
		NRenderUtils::NBuffer::Delete(Id);
}

void FUniformBuffer::FillSubData(const uint32 Offset, const uint32 RealDataSize, const void* Data)
{
	if(RealDataSize == 0) return;
	
	const uint32 correctedDataSize = NShaderUtils::NPrivate::CorrectUniformSizeType_Impl(RealDataSize);
	
	ENSURE_RET(Size > Offset);
	ENSURE_RET(Size - Offset >= correctedDataSize);

	const void* dataToSend = Data;
	if(RealDataSize < correctedDataSize)
	{
		// Correct also data if necessary
		// * allocate zeroed data
		uint8* newData = new uint8[correctedDataSize]();
		
		// TODO(kristian.fisera): Implement custom MEMCPY
		memcpy(newData, Data, RealDataSize);
		
		dataToSend = newData;
	}

	NRenderUtils::NBuffer::Bind(GL_UNIFORM_BUFFER, Id);
	glBufferSubData(GL_UNIFORM_BUFFER, Offset, correctedDataSize, dataToSend);
	NRenderUtils::NBuffer::Unbind(GL_UNIFORM_BUFFER);
	
	if(RealDataSize < correctedDataSize)
	{
		// Delete corrected data
		delete[] (uint8*)dataToSend;
	}
}
