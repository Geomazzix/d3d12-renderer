#pragma once
#include "Resources/IBuffer.h"

namespace mtd3d
{
	/// <summary>
	/// The vertex buffer acts as a resource wrapper, to identify and store a handle 
	/// that points to the vertices of a certain model.
	/// </summary>
	class VertexBuffer : public IBuffer
	{
	public:
		VertexBuffer(const std::wstring& name = L"");
		~VertexBuffer() = default;

		virtual void CreateViews(size_t numElements, size_t elementSize) override;

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

		size_t GetElementSize() const;
		size_t GetVertexCount() const;
		size_t GetVertexStride() const;

	private:
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
		size_t m_VertexCount;
	};
}