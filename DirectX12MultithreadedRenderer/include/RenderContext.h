#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include <vector>
#include <memory>

#include "RenderDevice.h"
#include "Window.h"

namespace mtd3d
{
	class ResizeEventArgs;
	class CommandQueue;
	class CommandList;

	/// <summary>
	/// The render context manages the swap chain.
	/// </summary>
	class RenderContext
	{
	public:
		RenderContext();
		~RenderContext();

		void Initialize(std::weak_ptr<RenderDevice> renderDevice, std::weak_ptr<CommandQueue> commandQueue, Window& window, int backBufferCount = 2, int msaaSampleCount = 4);
		void Destroy();

		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

		DXGI_FORMAT GetBackBufferFormat() const;
		DXGI_FORMAT GetDepthStencilFormat() const;

		void SetRSProperties(std::weak_ptr<CommandList> commandList);
		void Present();

		Microsoft::WRL::ComPtr<ID3D12Resource> GetCurrentBackBuffer();

	private:
		void CreateSwapChain(std::weak_ptr<CommandQueue> commandQueue, Window& window);
		void CheckMSAAQuality(int msaaSampleCount);

		void CreateRtvHeap();
		void CreateDsvHeap();

		void CreateRtvViews();
		void CreateDepthStencilView();

		void SetDisplayProperties(unsigned int clientWidth, unsigned int clientHeight);
		void OnClientResize(ResizeEventArgs& resizeEventArgs);

		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;
		bool m_IsTearingSupported;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
		unsigned int m_RtvDescriptorSize;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap;
		unsigned int m_DsvDescriptorSize;

		unsigned int m_CurrentSwapChainIndex;
		unsigned int m_SwapChainBufferCount;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_SwapChainBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;

		D3D12_VIEWPORT m_ViewPort;
		RECT m_ScissorRect;
		DXGI_SAMPLE_DESC m_SampleProperties;

		DXGI_FORMAT m_BackBufferFormat;
		DXGI_FORMAT m_DepthStencilFormat;

		std::weak_ptr<RenderDevice> m_RenderDevice;
		std::weak_ptr<CommandQueue> m_GraphicsCmdQueue;
	};
}