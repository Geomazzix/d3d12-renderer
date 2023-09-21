#include "RootSignature.h"
#include "Utility/D3DErrorHandler.h"

using namespace Microsoft::WRL;

namespace mtd3d
{

	RootSignature::RootSignature() :
		m_Desc({}),
		m_NumDescriptorsPerTable(),
		m_Version({})
	{}

	RootSignature::~RootSignature()
	{
		Destroy();
	}

	void RootSignature::Initialize(std::weak_ptr<RenderDevice> renderDevice, const D3D12_ROOT_SIGNATURE_DESC1& desc)
	{
		auto device = renderDevice.lock();
		if (device == nullptr)
		{
			printf("Root signature could not be created due to the renderDevice being: NULL\n");
			return;
		}

		UINT numParameters = desc.NumParameters;
		D3D12_ROOT_PARAMETER1* parameters = numParameters > 0 ? new D3D12_ROOT_PARAMETER1[numParameters] : nullptr;

		for (uint32_t i = 0; i < numParameters; i++)
		{
			const D3D12_ROOT_PARAMETER1& rootParam = desc.pParameters[i];
			parameters[i] = rootParam;

			if (rootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			{
				UINT numDescriptorRanges = rootParam.DescriptorTable.NumDescriptorRanges;
				D3D12_DESCRIPTOR_RANGE1* descriptorRanges = numDescriptorRanges > 0 ? new D3D12_DESCRIPTOR_RANGE1[numDescriptorRanges] : nullptr;

				memcpy(descriptorRanges, parameters[i].DescriptorTable.pDescriptorRanges, sizeof(D3D12_DESCRIPTOR_RANGE1) * numDescriptorRanges);

				parameters[i].DescriptorTable.NumDescriptorRanges = numDescriptorRanges;
				parameters[i].DescriptorTable.pDescriptorRanges = descriptorRanges;

				for (int j = 0; j < numDescriptorRanges; j++)
				{
					m_NumDescriptorsPerTable[i] += descriptorRanges[j].NumDescriptors;
				}
			}
		}

		m_Desc.NumParameters = numParameters;
		m_Desc.pParameters = parameters;

		UINT numStaticSamplers = desc.NumStaticSamplers;
		D3D12_STATIC_SAMPLER_DESC* staticSamplers = numStaticSamplers > 0 ? new D3D12_STATIC_SAMPLER_DESC[numParameters] : nullptr;

		if (staticSamplers != nullptr)
		{
			memcpy(staticSamplers, desc.pStaticSamplers, sizeof(D3D12_STATIC_SAMPLER_DESC) * numStaticSamplers);
		}

		m_Desc.NumStaticSamplers = numStaticSamplers;
		m_Desc.pStaticSamplers = staticSamplers;


		D3D12_ROOT_SIGNATURE_FLAGS flags = desc.Flags;
		//Optionally add forced flags.
		m_Desc.Flags = flags;

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(numParameters, parameters, numStaticSamplers, staticSamplers, flags);

		m_Version.HighestVersion = (device->Get()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &m_Version, sizeof(m_Version)) < 0)
			? D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1_0
			: D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1_1;

		ComPtr<ID3DBlob> rootSignatureBlob = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;

		HRESULT result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, m_Version.HighestVersion, &rootSignatureBlob, &errorBlob);
		if (errorBlob != nullptr) OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
		D3D_CHECK(result);

		D3D_CHECK(renderDevice.lock()->Get()->CreateRootSignature(
			0,
			rootSignatureBlob->GetBufferPointer(),
			rootSignatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&m_RootSignature))
		);
	}

	void RootSignature::Destroy()
	{
		for(int i = 0; i < m_Desc.NumParameters; i++)
		{
			const D3D12_ROOT_PARAMETER1& rootParameter = m_Desc.pParameters[i];
			if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			{
				delete[] rootParameter.DescriptorTable.pDescriptorRanges;
			}
		}

		delete[] m_Desc.pStaticSamplers;
		m_Desc.pStaticSamplers = nullptr;
		m_Desc.NumStaticSamplers = 0;

		delete[] m_Desc.pParameters;
		m_Desc.pParameters = nullptr;
		m_Desc.NumParameters = 0;

		memset(m_NumDescriptorsPerTable, 0, sizeof(m_NumDescriptorsPerTable));
	}

	Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignature::Get()
	{
		return m_RootSignature;
	}
}