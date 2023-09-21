#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace mtd3d
{
	/// <summary>
	/// Base class wrapper for an d3d12 resource.
	/// </summary>
	class IResource
	{
	public:
		IResource(const std::wstring& name = L"");
		IResource(Microsoft::WRL::ComPtr<ID3D12Device2> device, const D3D12_RESOURCE_DESC& desc, D3D12_CLEAR_VALUE* clearValue, const std::wstring& name = L"");
		IResource(Microsoft::WRL::ComPtr<ID3D12Device2> device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, const std::wstring& name = L"");

		IResource(const IResource& copy);
		IResource(IResource&& copy) noexcept;

		IResource& operator=(const IResource& other);
		IResource& operator=(IResource&& other) noexcept;

		virtual ~IResource() = default;
		virtual void Reset();

		bool IsValid() const;
		D3D12_RESOURCE_DESC GetDesc() const;

		void Set(Microsoft::WRL::ComPtr<ID3D12Device2> device, Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource, D3D12_CLEAR_VALUE* clearValue = nullptr);
		Microsoft::WRL::ComPtr<ID3D12Resource> Get() const;

		void SetName(const std::wstring& name);
		std::wstring GetName() const;

	protected:
		D3D12_FEATURE_DATA_FORMAT_SUPPORT m_FormatSupport;
		D3D12_CLEAR_VALUE* m_ClearValue;
		std::wstring m_ResourceName;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource;
	
	private:
		void CheckFeatureSupport(Microsoft::WRL::ComPtr<ID3D12Device2> device);
	};
}