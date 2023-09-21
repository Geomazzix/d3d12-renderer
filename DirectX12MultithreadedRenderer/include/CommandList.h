#pragma once

#include "RenderDevice.h"
#include "Utility/D3dx12.h"

#include "Resources/VertexBuffer.h"
#include "Resources/IndexBuffer.h"

#include <vector>
#include <cassert>
#include <wrl.h>
#include <d3d12.h>

namespace mtd3d
{
	/// <summary>
	/// The commandlist can be submitted to the renderqueue once finished recording.
	/// </summary>
	class CommandList
	{
	public:
		CommandList();
		~CommandList() = default;

		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
		void Destroy();

		D3D12_COMMAND_LIST_TYPE GetCommandListType() const;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> GetCmdList() const;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetCmdAllocator() const;

		void TransitionResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);

		void SetVertexBuffer(uint32_t slot, const VertexBuffer& vertexBuffer);
		void SetIndexBuffer(const IndexBuffer& indexBuffer);

		void CopyVertexBuffer(Microsoft::WRL::ComPtr<ID3D12Device2> device, VertexBuffer& vertexBuffer, size_t numVertices, size_t vertexStride, const void* vertexBufferData);
		template<typename T>
		void CopyVertexBuffer(Microsoft::WRL::ComPtr<ID3D12Device2> device, VertexBuffer& vertexBuffer, const std::vector<T>& vertexBufferData);

		void CopyIndexBuffer(Microsoft::WRL::ComPtr<ID3D12Device2> device, IndexBuffer& indexBuffer, size_t numIndicies, DXGI_FORMAT indexFormat, const void* indexBufferData);
		template<typename T>
		void CopyIndexBuffer(Microsoft::WRL::ComPtr<ID3D12Device2> device, IndexBuffer& indexBuffer, const std::vector<T>& indexBufferData);

		void SetPipelineStateObject(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateObject);
		void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology);

		void RecordIndexedDraw(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance);
		void RecordDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance);

		void Close();
		void Reset();
		void Reset(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator);

	private:
		void CopyBuffer(Microsoft::WRL::ComPtr<ID3D12Device2> device, IBuffer& buffer, size_t numElements, size_t elementSize, const void* bufferData, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE);
		void TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> object);
		void ReleaseTrackedObjects();

		std::vector<Microsoft::WRL::ComPtr<ID3D12Object>> m_TrackedObjects;

		D3D12_COMMAND_LIST_TYPE m_Type;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> m_CommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_Pso;
	};

	template<typename T>
	void CommandList::CopyIndexBuffer(Microsoft::WRL::ComPtr<ID3D12Device2> device, IndexBuffer& indexBuffer, const std::vector<T>& indexBufferData)
	{
		assert(sizeof(T) == 2 || sizeof(T) == 4); //Assert when using an unsupported format, 16 or 32 bit integers are supported.

		DXGI_FORMAT indexFormat = (sizeof(T) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		CopyIndexBuffer(device, indexBuffer, indexBufferData.size(), indexFormat, indexBufferData.data());
	}

	template<typename T>
	void CommandList::CopyVertexBuffer(Microsoft::WRL::ComPtr<ID3D12Device2> device, VertexBuffer& vertexBuffer, const std::vector<T>& vertexBufferData)
	{
		CopyVertexBuffer(device, vertexBuffer, vertexBufferData.size(), sizeof(T), vertexBufferData.data());
	}
}