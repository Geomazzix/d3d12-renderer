#include "CommandList.h"
#include "Utility/D3DErrorHandler.h"

namespace mtd3d
{
	CommandList::CommandList() : 
		m_Type(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT),
		m_Pso(nullptr)
	{}

	void CommandList::Initialize(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
	{
		D3D_CHECK(device->CreateCommandAllocator(type, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())));
		D3D_CHECK(device->CreateCommandList(0, type, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList)));
		m_CommandList->Close();
	}

	void CommandList::Destroy()
	{
		Close();
	}

	D3D12_COMMAND_LIST_TYPE CommandList::GetCommandListType() const
	{
		return m_Type;
	}

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CommandList::GetCmdList() const
	{
		return m_CommandList;
	}

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandList::GetCmdAllocator() const
	{
		return m_CommandAllocator;
	}

	void CommandList::TransitionResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), beforeState, afterState);
		m_CommandList->ResourceBarrier(1, &barrier);
	}

	void CommandList::SetVertexBuffer(uint32_t slot, const VertexBuffer& vertexBuffer)
	{
		auto vertexBufferView = vertexBuffer.GetVertexBufferView();
		m_CommandList->IASetVertexBuffers(slot, 1, &vertexBufferView);
		TrackResource(vertexBuffer.Get());
	}

	void CommandList::SetIndexBuffer(const IndexBuffer& indexBuffer)
	{
		auto indexBufferView = indexBuffer.GetIndexBufferView();
		m_CommandList->IASetIndexBuffer(&indexBufferView);
		TrackResource(indexBuffer.Get());
	}

	void CommandList::CopyVertexBuffer(Microsoft::WRL::ComPtr<ID3D12Device2> device, VertexBuffer& vertexBuffer, size_t numVertices, size_t vertexStride, const void* vertexBufferData)
	{
		CopyBuffer(device, vertexBuffer, numVertices, vertexStride, vertexBufferData);
	}

	void CommandList::CopyIndexBuffer(Microsoft::WRL::ComPtr<ID3D12Device2> device, IndexBuffer& indexBuffer, size_t numIndicies, DXGI_FORMAT indexFormat, const void* indexBufferData)
	{
		size_t indexSizeInBytes = indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4;
		CopyBuffer(device, indexBuffer, numIndicies, indexSizeInBytes, indexBufferData);
	}

	void CommandList::SetPipelineStateObject(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateObject)
	{
		m_CommandList->SetPipelineState(pipelineStateObject.Get());
		m_Pso = pipelineStateObject;
	}

	void CommandList::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology)
	{
		m_CommandList->IASetPrimitiveTopology(topology);
	}

	void CommandList::RecordIndexedDraw(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance)
	{
		m_CommandList->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex, startInstance);
	}

	void CommandList::RecordDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance)
	{
		m_CommandList->DrawInstanced(vertexCount, instanceCount, startVertex, startInstance);
	}

	void CommandList::Close()
	{
		D3D_CHECK(m_CommandList->Close());
	}

	void CommandList::Reset(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator)
	{
		m_CommandList->Reset(commandAllocator.Get(), m_Pso.Get());		
		ReleaseTrackedObjects();
	}

	void CommandList::Reset()
	{
		D3D_CHECK(m_CommandAllocator->Reset());
		m_CommandList->Reset(m_CommandAllocator.Get(), m_Pso.Get());
		ReleaseTrackedObjects();
	}

	void CommandList::CopyBuffer(Microsoft::WRL::ComPtr<ID3D12Device2> device, IBuffer& buffer, size_t numElements, size_t elementSize, const void* bufferData, D3D12_RESOURCE_FLAGS flags /*= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE*/)
	{
		LONG_PTR bufferSizeInBytes = numElements * elementSize;
		
		//Create the destination buffer.
		CD3DX12_HEAP_PROPERTIES bufferProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSizeInBytes, flags);
		Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;

		D3D_CHECK(device->CreateCommittedResource(
			&bufferProps,
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(defaultBuffer.GetAddressOf())
		));

		//Create an intermediate buffer, used to upload the data into the destination buffer.
		CD3DX12_HEAP_PROPERTIES uploadBufferProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC uploadResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSizeInBytes);
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;

		D3D_CHECK(device->CreateCommittedResource(
			&uploadBufferProps,
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&uploadResourceDesc,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.GetAddressOf())
		));

		//Before copying the data over, switch the state to copy_destination, so the buffer is actually prepared to take data in. Revert this after the data has been passed in.
		D3D12_SUBRESOURCE_DATA subResData =
		{
			bufferData,
			bufferSizeInBytes,
			bufferSizeInBytes
		};

		TransitionResource(defaultBuffer.Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST);
		UpdateSubresources(m_CommandList.Get(), defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResData);
		TransitionResource(defaultBuffer.Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ);
		
		//Make sure to track the buffers so the ComPtr ref count won't reach 0.
		TrackResource(uploadBuffer.Get());
		TrackResource(defaultBuffer.Get());

		//Initialize the buffer the data has been copied for.
		buffer.Set(device, defaultBuffer);
		buffer.CreateViews(numElements, elementSize);
	}

	void CommandList::TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> object)
	{
		m_TrackedObjects.push_back(object);
	}

	void CommandList::ReleaseTrackedObjects()
	{
		m_TrackedObjects.clear();
	}
}