#pragma once
#include "Resources/IBuffer.h"
#include <string>

namespace mtd3d
{
	/// <summary>
	/// The index buffer acts as a resource wrapper, to identify and store a handle 
	/// that points to the indicies of a certain model.
	/// </summary>
	class IndexBuffer : public IBuffer
	{
	public:
		IndexBuffer(const std::wstring& name = L"");
		~IndexBuffer() = default;

		virtual void CreateViews(size_t numElements, size_t elementSize) override;

		size_t GetElementSize() const;
		size_t GetNumIndicies() const;
		DXGI_FORMAT GetIndexFormat() const;
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

	private:
		size_t m_IndexCount;
		DXGI_FORMAT m_IndexFormat;
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	};
}