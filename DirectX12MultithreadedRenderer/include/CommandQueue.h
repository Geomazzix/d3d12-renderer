#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include <queue>

namespace mtd3d
{
	class RenderDevice;
	class CommandList;

	/// <summary>
	/// The commandqueue represents the commands that will be send to the GPU whenever the swapchain->present gets called.
	/// </summary>
	class CommandQueue
	{
	public:
		CommandQueue();
		~CommandQueue() = default;

		void Initialize(RenderDevice& renderDevice, D3D12_COMMAND_LIST_TYPE type);

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> Get();

		std::weak_ptr<CommandList> GetCommandList();
		std::weak_ptr<CommandList> GetCommandList(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator);
		void ExecuteCommandList(std::weak_ptr<CommandList> commandList);

		void Signal();
		void WaitOnFenceValue(UINT64 fenceValue);
		void Flush();

		UINT64 GetCompletedFenceValue() const;
		UINT64 GetFenceValue() const;

	private:
		std::shared_ptr<CommandList> m_CommandList;

		UINT64 m_FenceValue;
		Microsoft::WRL::ComPtr<ID3D12Fence1> m_Fence;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
	};
}