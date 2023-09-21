#include "Resources/IndexBuffer.h"
#include <cassert>

namespace mtd3d
{

	IndexBuffer::IndexBuffer(const std::wstring& name /*= L""*/) :
		IBuffer(name),
		m_IndexCount(0),
		m_IndexFormat(DXGI_FORMAT::DXGI_FORMAT_R16_UINT),
		m_IndexBufferView({}) 
	{}

	void IndexBuffer::CreateViews(size_t numElements, size_t elementSize)
	{
		assert(elementSize == 2 || elementSize == 4 && "Indices must be 16 or 32-bit integers!");

		m_IndexCount = numElements;
		m_IndexFormat = (elementSize == 2) 
			? DXGI_FORMAT::DXGI_FORMAT_R16_UINT 
			: DXGI_FORMAT::DXGI_FORMAT_R32_UINT;

		m_IndexBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
		m_IndexBufferView.Format = m_IndexFormat;
		m_IndexBufferView.SizeInBytes = static_cast<unsigned int>(numElements * elementSize);
	}

	size_t IndexBuffer::GetElementSize() const
	{
		return m_IndexBufferView.SizeInBytes / m_IndexCount;
	}

	size_t IndexBuffer::GetNumIndicies() const
	{
		return m_IndexCount;
	}

	DXGI_FORMAT IndexBuffer::GetIndexFormat() const
	{
		return m_IndexFormat;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBuffer::GetIndexBufferView() const
	{
		return m_IndexBufferView;
	}
}