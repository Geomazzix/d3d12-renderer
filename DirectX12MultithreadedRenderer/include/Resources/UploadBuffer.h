#pragma once

#include <d3d12.h>
#include <wrl.h>
#include "Utility/D3DErrorHandler.h"
#include "Utility/D3dx12.h"

#pragma warning (push)
#pragma warning (disable : 26451)

namespace mtd3d
{
	/// <summary>
	/// This helper class is used to copy data into buffers to the GPU.
	/// </summary>
	template<typename Data>
	class UploadBuffer
	{
	public:
		UploadBuffer(Microsoft::WRL::ComPtr<ID3D12Device2> device, UINT elementCount, bool isConstantBuffer);
		~UploadBuffer();

		UploadBuffer(const UploadBuffer& rhs) = delete;
		UploadBuffer& operator=(const UploadBuffer& rhs) = delete;

		Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() const;
		void CopyData(int elementIndex, const Data data);

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_UploadBuffer;
		BYTE* m_MappedData;

		UINT m_ElementByteSize;
		bool m_IsConstantBuffer;
	};

	template<typename Data>
	UploadBuffer<Data>::UploadBuffer(Microsoft::WRL::ComPtr<ID3D12Device2> device, UINT elementCount, bool isConstantBuffer) :
		m_MappedData(nullptr),
		m_ElementByteSize(0),
		m_IsConstantBuffer(false)
	{
		m_ElementByteSize = sizeof(Data);

		if (isConstantBuffer)
		{
			m_ElementByteSize = (sizeof(Data) + 255) & ~255;
		}

		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_ElementByteSize * elementCount);

		D3D_CHECK(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_UploadBuffer)
		));

		D3D_CHECK(m_UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedData)));
	}

	template<typename Data>
	UploadBuffer<Data>::~UploadBuffer()
	{
		if (m_UploadBuffer != nullptr)
			m_UploadBuffer->Unmap(0, nullptr);
		m_MappedData = nullptr;
	}

	template<typename Data>
	void UploadBuffer<Data>::CopyData(int elementIndex, const Data data)
	{
		memcpy(&m_MappedData[elementIndex * m_ElementByteSize], &data, sizeof(Data));
	}

	template<typename Data>
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadBuffer<Data>::GetResource() const
	{
		return m_UploadBuffer.Get();
	}
}

#pragma warning (pop)