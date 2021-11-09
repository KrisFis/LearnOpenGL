
#include "UniformBuffer.h"
#include "RenderUtils.h"

FUniformBuffer::FUniformBuffer(uint32 InPointIdx, uint32 InSize)
	: Id(0)
	, PointIdx(InPointIdx)
	, Size(InSize)
{
	Id = NRenderUtils::NBuffer::Generate();
	NRenderUtils::NBuffer::Bind(GL_UNIFORM_BUFFER, Id);
	
	glBufferData(GL_UNIFORM_BUFFER, Size, nullptr, GL_STATIC_DRAW);
	
	NRenderUtils::NBuffer::Unbind(GL_UNIFORM_BUFFER);
	
	glBindBufferRange(GL_UNIFORM_BUFFER, PointIdx, Id, 0, Size);
}

FUniformBuffer::~FUniformBuffer()
{
	if(Id != NRenderConsts::Invalid::BufferId)
		NRenderUtils::NBuffer::Delete(Id);
}

void FUniformBuffer::FillSubData(uint32 Offset, uint32 DataSize, const void* Data)
{
	ENSURE_RET(Size > Offset);
	ENSURE_RET(Size - Offset >= DataSize);

	NRenderUtils::NBuffer::Bind(GL_UNIFORM_BUFFER, Id);
	glBufferSubData(GL_UNIFORM_BUFFER, Offset, DataSize, Data);
	NRenderUtils::NBuffer::Unbind(GL_UNIFORM_BUFFER);
}
