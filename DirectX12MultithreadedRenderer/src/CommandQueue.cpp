#include "CommandQueue.h"
#include "Utility/D3DErrorHandler.h"
#include "RenderDevice.h"
#include "Utility/D3dx12.h"
#include "CommandList.h"

using namespace Microsoft::WRL;

namespace mtd3d
{

	CommandQueue::CommandQueue() : m_FenceValue(0) {}

	void CommandQueue::Initialize(RenderDevice& renderDevice, D3D12_COMMAND_LIST_TYPE type)
	{
		m_CommandList = std::make_shared<CommandList>();

		D3D12_COMMAND_QUEUE_DESC commandQueueInfo =
		{
			type,
			0,
			D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE,
			0
		};

		D3D_CHECK(renderDevice.Get()->CreateCommandQueue(&commandQueueInfo, IID_PPV_ARGS(&m_CommandQueue)));
		D3D_CHECK(renderDevice.Get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
		
		m_CommandList->Initialize(renderDevice.Get(), type);
	}

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue::Get()
	{
		return m_CommandQueue;
	}

	std::weak_ptr<CommandList> CommandQueue::GetCommandList(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator)
	{
		m_CommandList->Reset(commandAllocator);
		return m_CommandList;
	}

	std::weak_ptr<CommandList> CommandQueue::GetCommandList()
	{
		m_CommandList->Reset();
		return m_CommandList;
	}

	void CommandQueue::ExecuteCommandList(std::weak_ptr<CommandList> commandList)
	{
		commandList.lock()->Close();

		ID3D12CommandList* cmdsLists[] =
		{
			commandList.lock()->GetCmdList().Get()
		};

		m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	}

	void CommandQueue::Signal()
	{
		++m_FenceValue;
		D3D_CHECK(m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue));
	}

	void CommandQueue::WaitOnFenceValue(UINT64 fenceValue)
	{
		if (fenceValue != 0 && m_Fence->GetCompletedValue() < fenceValue)
		{
			HANDLE eventHandle = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
			if (eventHandle == 0)
			{
				printf("Failed to create the fence event handle!");
				return;
			}

			D3D_CHECK(m_Fence->SetEventOnCompletion(fenceValue, eventHandle));
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	void CommandQueue::Flush()
	{
		m_FenceValue++;

		D3D_CHECK(m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue));
		if (m_Fence->GetCompletedValue() < m_FenceValue)
		{
			HANDLE eventHandle = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
			if (eventHandle == 0)
			{
				printf("Failed to create the fence event handle!");
				return;
			}

			m_Fence->SetEventOnCompletion(m_FenceValue, eventHandle);
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	UINT64 CommandQueue::GetCompletedFenceValue() const
	{
		return m_Fence->GetCompletedValue();
	}

	UINT64 CommandQueue::GetFenceValue() const
	{
		return m_FenceValue;
	}

}