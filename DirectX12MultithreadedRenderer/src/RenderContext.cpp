#include "RenderContext.h"

#include "CommandQueue.h"
#include "CommandList.h"

#include "Utility/D3DErrorHandler.h"
#include "Utility/D3dx12.h"

#include "EventSystem/EventArgs.h"
#include "EventSystem/EventMessenger.h"

#include <cassert>

using namespace Microsoft::WRL;

namespace mtd3d
{
	RenderContext::RenderContext() :
		m_DsvDescriptorSize(0),
		m_SampleProperties({1, 0}),
		m_CurrentSwapChainIndex(0),
		m_ViewPort({}),
		m_SwapChainBufferCount(2),
		m_ScissorRect({}),
		m_IsTearingSupported(false),
		m_RtvDescriptorSize(0),
		m_BackBufferFormat(DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM),
		m_DepthStencilFormat(DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT)
	{
		EventMessenger::GetInstance().ConnectListener<ResizeEventArgs&>("OnWindowResize", &RenderContext::OnClientResize, this);
	}

	RenderContext::~RenderContext()
	{
		EventMessenger::GetInstance().DisconnectListener<ResizeEventArgs&>("OnWindowResize", &RenderContext::OnClientResize, this);
	}

	void RenderContext::Initialize(std::weak_ptr<RenderDevice> renderDevice, std::weak_ptr<CommandQueue> commandQueue, Window& window, int backBufferCount /* = 2*/, int msaaSampleCount /*= 4*/)
	{
		m_SwapChainBufferCount = backBufferCount;
		m_RenderDevice = renderDevice;
		m_GraphicsCmdQueue = commandQueue;

		SetDisplayProperties(window.GetWindowWidth(), window.GetWindowHeight());

		CheckMSAAQuality(msaaSampleCount);
		CreateSwapChain(commandQueue, window);

		CreateRtvHeap();
		CreateDsvHeap();

		CreateRtvViews();
		CreateDepthStencilView();
	}

	void RenderContext::Destroy()
	{

	}

	D3D12_CPU_DESCRIPTOR_HANDLE RenderContext::GetCurrentBackBufferView() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentSwapChainIndex, m_RtvDescriptorSize);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RenderContext::GetDepthStencilView() const
	{
		return m_DsvHeap->GetCPUDescriptorHandleForHeapStart();
	}

	DXGI_FORMAT RenderContext::GetBackBufferFormat() const
	{
		return m_BackBufferFormat;
	}

	DXGI_FORMAT RenderContext::GetDepthStencilFormat() const
	{
		return m_DepthStencilFormat;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> RenderContext::GetCurrentBackBuffer()
	{
		return m_SwapChainBuffer[m_CurrentSwapChainIndex];
	}

	void RenderContext::CreateSwapChain(std::weak_ptr<CommandQueue> commandQueue, Window& window)
	{
		ComPtr<IDXGIFactory4> factory4;
		HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory4));
		BOOL allowTearing = FALSE;
		if (SUCCEEDED(hr))
		{
			ComPtr<IDXGIFactory5> factory5;
			hr = factory4.As(&factory5);
			if (SUCCEEDED(hr))
			{
				hr = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
			}
		}
		m_IsTearingSupported = SUCCEEDED(hr) && allowTearing;

		DXGI_SAMPLE_DESC smDesc =
		{
			m_SampleProperties.Quality,
			m_SampleProperties.Quality == 4 ? m_SampleProperties.Quality - 1 : 0
		};

		ComPtr<IDXGIFactory4> dxgiFactory4;
		UINT createFactoryFlags = 0;

#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		D3D_CHECK(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

		UINT swapChainFlags = m_IsTearingSupported ? DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = window.GetWindowWidth();
		swapChainDesc.Height = window.GetWindowHeight();
		swapChainDesc.Format = m_BackBufferFormat;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc = { 1, 0 };
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = m_SwapChainBufferCount;
		swapChainDesc.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Flags = swapChainFlags | DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		ComPtr<IDXGISwapChain1> swapChain1;
		D3D_CHECK(dxgiFactory4->CreateSwapChainForHwnd(
			commandQueue.lock()->Get().Get(),
			window.GetWindowHandle(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1
		));
		D3D_CHECK(swapChain1.As(&m_SwapChain));
	}

	void RenderContext::CheckMSAAQuality(int msaaSampleCount)
	{
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = m_BackBufferFormat;
		msQualityLevels.SampleCount = 4;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS::D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		D3D_CHECK(m_RenderDevice.lock()->Get()->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)));

		bool msaaState = msQualityLevels.NumQualityLevels > 0;
		m_SampleProperties.Count = msaaState ? 4 : 1;
		m_SampleProperties.Quality = msaaState ? (msQualityLevels.NumQualityLevels - 1) : 0;
	}

	void RenderContext::SetRSProperties(std::weak_ptr<CommandList> commandList)
	{
		commandList.lock()->GetCmdList()->RSSetViewports(1, &m_ViewPort);
		commandList.lock()->GetCmdList()->RSSetScissorRects(1, &m_ScissorRect);
	}

	void RenderContext::Present()
	{
		UINT presentFlags = m_IsTearingSupported ? DXGI_PRESENT_ALLOW_TEARING : 0;
		D3D_CHECK(m_SwapChain->Present(0, presentFlags));
		m_CurrentSwapChainIndex = (m_CurrentSwapChainIndex + 1) % m_SwapChainBufferCount;
	}

	void RenderContext::CreateRtvHeap()
	{
		m_RtvDescriptorSize = m_RenderDevice.lock()->Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc =
		{
			D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			m_SwapChainBufferCount,
			D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			0
		};

		D3D_CHECK(m_RenderDevice.lock()->Get()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_RtvHeap.GetAddressOf())));
	}

	void RenderContext::CreateDsvHeap()
	{
		m_DsvDescriptorSize = m_RenderDevice.lock()->Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc =
		{
			D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			1,
			D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			0
		};

		D3D_CHECK(m_RenderDevice.lock()->Get()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_DsvHeap.GetAddressOf())));
	}

	void RenderContext::CreateRtvViews()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
		m_SwapChainBuffer.resize(m_SwapChainBufferCount);

		for (unsigned int i = 0; i < m_SwapChainBufferCount; ++i)
		{
			D3D_CHECK(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffer[i])));
			m_RenderDevice.lock()->Get()->CreateRenderTargetView(m_SwapChainBuffer[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_RtvDescriptorSize);
		}
	}

	void RenderContext::CreateDepthStencilView()
	{
		D3D12_RESOURCE_DESC depthStencilDesc =
		{
			D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			0,
			static_cast<UINT64>(m_ViewPort.Width),
			static_cast<UINT>(m_ViewPort.Height),
			1,
			1,
			m_DepthStencilFormat,
			{ 1, 0 },
			D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN,
			D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		};

		D3D12_CLEAR_VALUE optClear =
		{
			m_DepthStencilFormat,
			{ 
				1.0f, 
				0.0f 
			}
		};

		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT);

		D3D_CHECK(m_RenderDevice.lock()->Get()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optClear,
			IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf())
		));

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc =
		{
			DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT,
			D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D,
			D3D12_DSV_FLAGS::D3D12_DSV_FLAG_NONE
		};
		dsvDesc.Texture2D.MipSlice = 0;
		m_RenderDevice.lock()->Get()->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &dsvDesc, GetDepthStencilView());
	}

	void RenderContext::SetDisplayProperties(unsigned int clientWidth, unsigned int clientHeight)
	{
		m_ViewPort =
		{
			0,
			0,
			static_cast<float>(clientWidth),
			static_cast<float>(clientHeight),
			0.0f,
			1.0f
		};

		m_ScissorRect =
		{
			0,
			0,
			static_cast<long>(clientWidth),
			static_cast<long>(clientHeight)
		};
	}

	void RenderContext::OnClientResize(ResizeEventArgs& resizeEventArgs)
	{
		assert(m_GraphicsCmdQueue.lock() && "The graphics command queue doesn't exist anymore!");

		std::shared_ptr<CommandQueue> cmdQueue = m_GraphicsCmdQueue.lock();
		cmdQueue->Flush();
		std::weak_ptr<CommandList> cmdList = cmdQueue->GetCommandList();

		for(unsigned int i = 0; i < m_SwapChainBufferCount; ++i)
			m_SwapChainBuffer[i].Reset();
		m_DepthStencilBuffer.Reset();

		UINT swapChainFlags = m_IsTearingSupported ? DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		D3D_CHECK(m_SwapChain->ResizeBuffers(
			m_SwapChainBufferCount, 
			resizeEventArgs.Width, 
			resizeEventArgs.Height, 
			m_BackBufferFormat, 
			swapChainFlags | DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		));

		m_CurrentSwapChainIndex = 0;

		SetDisplayProperties(resizeEventArgs.Width, resizeEventArgs.Height);

		CreateRtvViews();
		CreateDepthStencilView();

		cmdQueue->ExecuteCommandList(cmdList);
		cmdQueue->Flush();
	}
}