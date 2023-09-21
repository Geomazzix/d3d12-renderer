#include "Resources/VertexBuffer.h"

namespace mtd3d
{
	VertexBuffer::VertexBuffer(const std::wstring& name /*= L""*/) :
		IBuffer(name),
		m_VertexCount(0),
		m_VertexBufferView({})
	{}

	void VertexBuffer::CreateViews(size_t numElements, size_t elementSize)
	{
		m_VertexCount = numElements;

		m_VertexBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = static_cast<unsigned int>(numElements * elementSize);
		m_VertexBufferView.StrideInBytes = static_cast<unsigned int>(elementSize);
	}

	D3D12_VERTEX_BUFFER_VIEW VertexBuffer::GetVertexBufferView() const
	{
		return m_VertexBufferView;
	}

	size_t VertexBuffer::GetElementSize() const
	{
		return m_VertexBufferView.SizeInBytes / m_VertexCount;
	}

	size_t VertexBuffer::GetVertexCount() const
	{
		return m_VertexCount;
	}

	size_t VertexBuffer::GetVertexStride() const
	{
		return m_VertexBufferView.StrideInBytes;
	}
}