#pragma once
#include <wrl.h>
#include <memory>
#include <dxgi1_6.h>
#include <memory>

#include "Utility/D3DErrorHandler.h"
#include "CommandQueue.h"
#include "PipelineStateObject.h"

namespace mtd3d
{
	/// <summary>
	/// The render device manages the d3d12 instance of the API.
	/// </summary>
	class RenderDevice
	{
	public:
		RenderDevice() = default;
		~RenderDevice() = default;

		Microsoft::WRL::ComPtr<ID3D12Device2> Get() const;
		std::weak_ptr<CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const;

		void Initialize();

		template<typename PipelineStateStream>
		std::shared_ptr<PipelineStateObject> CreatePipelineStateObject(PipelineStateStream& pipelineStateStream);

		void Flush();

	private:
		std::unique_ptr<D3DErrorHandler> m_ErrorHandler;
		Microsoft::WRL::ComPtr<ID3D12Device2> m_Device;

		std::shared_ptr<CommandQueue> m_DirectCommandQueue;
		std::shared_ptr<CommandQueue> m_CopyCommandQueue;
		std::shared_ptr<CommandQueue> m_ComputeCommandQueue;
	};

	template<typename PipelineStateStream>
	std::shared_ptr<PipelineStateObject> RenderDevice::CreatePipelineStateObject(PipelineStateStream& pipelineStateStream)
	{
		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(PipelineStateStream), &pipelineStateStream };

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		pso->Initialize(*this, pipelineStateStreamDesc);
		return pso;
	}
}