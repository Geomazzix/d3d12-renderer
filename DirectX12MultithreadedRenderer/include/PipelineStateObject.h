#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <memory>

namespace mtd3d
{
	class RenderDevice;

	/// <summary>
	/// Brief wrapper for the d3d12 PSO.
	/// </summary>
	class PipelineStateObject
	{
	public:
		PipelineStateObject() = default;
		~PipelineStateObject() = default;
	
		void Initialize(RenderDevice&, const D3D12_PIPELINE_STATE_STREAM_DESC& desc);

		Microsoft::WRL::ComPtr<ID3D12PipelineState> Get() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSO;
	};
}