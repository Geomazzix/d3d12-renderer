#include "PipelineStateObject.h"
#include "Utility/D3DErrorHandler.h"
#include "RenderDevice.h"

using namespace Microsoft::WRL;

namespace mtd3d
{
	void PipelineStateObject::Initialize(RenderDevice& renderDevice, const D3D12_PIPELINE_STATE_STREAM_DESC& desc)
	{
		D3D_CHECK(renderDevice.Get()->CreatePipelineState(&desc, IID_PPV_ARGS(m_PSO.GetAddressOf())));
	}

	ComPtr<ID3D12PipelineState> PipelineStateObject::Get() const
	{
		return m_PSO;
	}
}