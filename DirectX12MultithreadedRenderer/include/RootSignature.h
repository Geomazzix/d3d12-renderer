#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "RenderDevice.h"
#include "Utility/D3dx12.h"

//Main approach taken from Jeremiah van Oosten: https://github.com/jpvanoosten/LearningDirectX12

namespace mtd3d
{
	/// <summary>
	/// Serves as a wrapper over the ID3D12RootSignature.
	/// </summary>
	class RootSignature
	{
	public:
		RootSignature();
		~RootSignature();

		void Initialize(std::weak_ptr<RenderDevice> renderDevice, const D3D12_ROOT_SIGNATURE_DESC1& desc);
		void Destroy();

		Microsoft::WRL::ComPtr<ID3D12RootSignature> Get();

	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		D3D12_FEATURE_DATA_ROOT_SIGNATURE m_Version;
		D3D12_ROOT_SIGNATURE_DESC1 m_Desc;
		uint32_t m_NumDescriptorsPerTable[32];
	};
}