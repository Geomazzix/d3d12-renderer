#include "RenderDevice.h"
#include <cassert>

using namespace Microsoft::WRL;

namespace mtd3d
{
	void RenderDevice::Initialize()
	{
		m_ErrorHandler = std::make_unique<D3DErrorHandler>();
		ComPtr<IDXGIFactory4> factory;

		unsigned int dxgiFactoryFlags = 0;
#if defined (DEBUG) || defined(_DEBUG)
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
		D3D_CHECK(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

		HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));
		if(FAILED(hardwareResult))
		{
			printf("Warning: Failed to select the default hardware adapter, falling back to use the WARP adapter.");
			ComPtr<IDXGIAdapter> warpAdapter;
			D3D_CHECK(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
			D3D_CHECK(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));
		}

#if defined(DEBUG) || defined(_DEBUG)
		Microsoft::WRL::ComPtr<ID3D12InfoQueue> pInfoQueue;
		if (SUCCEEDED(m_Device.As(&pInfoQueue)))
		{
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

			// Suppress whole categories of messages
			//D3D12_MESSAGE_CATEGORY Categories[] = {};

			// Suppress messages based on their severity level
			D3D12_MESSAGE_SEVERITY Severities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO
			};

			// Suppress individual messages by their ID
			D3D12_MESSAGE_ID DenyIds[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
			};

			D3D12_INFO_QUEUE_FILTER NewFilter = {};
			//NewFilter.DenyList.NumCategories = _countof(Categories);
			//NewFilter.DenyList.pCategoryList = Categories;
			NewFilter.DenyList.NumSeverities = _countof(Severities);
			NewFilter.DenyList.pSeverityList = Severities;
			NewFilter.DenyList.NumIDs = _countof(DenyIds);
			NewFilter.DenyList.pIDList = DenyIds;

			D3D_CHECK(pInfoQueue->PushStorageFilter(&NewFilter));
		}
#endif

		m_DirectCommandQueue = std::make_shared<CommandQueue>();
		m_CopyCommandQueue = std::make_shared<CommandQueue>();
		m_ComputeCommandQueue = std::make_shared<CommandQueue>();

		m_DirectCommandQueue->Initialize(*this, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_CopyCommandQueue->Initialize(*this, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COPY);
		m_ComputeCommandQueue->Initialize(*this, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE);
	}

	void RenderDevice::Flush()
	{
		m_DirectCommandQueue->Flush();
		m_ComputeCommandQueue->Flush();
		m_CopyCommandQueue->Flush();
	}

	Microsoft::WRL::ComPtr<ID3D12Device2> RenderDevice::Get() const
	{
		return m_Device;
	}

	std::weak_ptr<mtd3d::CommandQueue> RenderDevice::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const
	{
		std::weak_ptr<CommandQueue> commandQueue;
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			commandQueue = m_DirectCommandQueue;
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			commandQueue = m_ComputeCommandQueue;
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			commandQueue = m_CopyCommandQueue;
			break;
		default:
			assert(false && "Invalid command queue type.");
		}

		return commandQueue;
	}
}