#include "Resources/IResource.h"
#include "Utility/D3DErrorHandler.h"
#include <Utility/D3dx12.h>

namespace mtd3d
{
	IResource::IResource(const std::wstring& name /*= L""*/)
	{
		m_ResourceName = name;
		m_FormatSupport = {};
	}

	IResource::IResource(Microsoft::WRL::ComPtr<ID3D12Device2> device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, const std::wstring& name /*= L""*/) :
		m_Resource(resource),
		m_FormatSupport({})
	{
		CheckFeatureSupport(device);
		SetName(name);
	}

	IResource::IResource(const IResource& copy)
	{
		m_ResourceName = copy.m_ResourceName;
		m_Resource = copy.m_Resource;
		m_FormatSupport = copy.m_FormatSupport;
		m_ClearValue = copy.m_ClearValue != nullptr ? new D3D12_CLEAR_VALUE(*copy.m_ClearValue) : nullptr;
	}

	IResource::IResource(IResource&& copy) noexcept
	{
		m_Resource = std::move(copy.m_Resource);
		m_FormatSupport = std::move(copy.m_FormatSupport);
		m_ResourceName = std::move(copy.m_ResourceName);
		m_ClearValue = std::move(copy.m_ClearValue);
	}

	IResource::IResource(Microsoft::WRL::ComPtr<ID3D12Device2> device, const D3D12_RESOURCE_DESC& desc, D3D12_CLEAR_VALUE* clearValue, const std::wstring& name /*= L""*/)
	{
		m_ResourceName = name;

		if (clearValue != nullptr)
		{
			m_ClearValue = clearValue;
		}

		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		D3D_CHECK(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON,
			m_ClearValue,
			IID_PPV_ARGS(&m_Resource))
		);

		CheckFeatureSupport(device);
		SetName(name);
	}

	mtd3d::IResource& IResource::operator=(IResource&& other) noexcept
	{
		if (this != &other)
		{
			m_Resource = std::move(other.m_Resource);
			m_FormatSupport = other.m_FormatSupport;
			m_ResourceName = std::move(other.m_ResourceName);
			m_ClearValue = std::move(other.m_ClearValue);

			other.Reset();
		}

		return *this;
	}

	mtd3d::IResource& IResource::operator=(const IResource& other)
	{
		if (this != &other)
		{
			m_Resource = other.m_Resource;
			m_FormatSupport = other.m_FormatSupport;
			m_ResourceName = other.m_ResourceName;

			if (other.m_ClearValue != nullptr)
			{
				m_ClearValue = new D3D12_CLEAR_VALUE(*other.m_ClearValue);
			}
		}

		return *this;
	}

	bool IResource::IsValid() const
	{
		return m_Resource != nullptr;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> IResource::Get() const
	{
		return m_Resource;
	}

	D3D12_RESOURCE_DESC IResource::GetDesc() const
	{
		return m_Resource != nullptr ? m_Resource->GetDesc() : D3D12_RESOURCE_DESC();
	}

	void IResource::Set(Microsoft::WRL::ComPtr<ID3D12Device2> device, Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource, D3D12_CLEAR_VALUE* clearValue /*= nullptr*/)
	{
		m_Resource = d3d12Resource;
		m_ClearValue = m_ClearValue != nullptr ? clearValue : nullptr;

		CheckFeatureSupport(device);
		SetName(m_ResourceName);
	}

	void IResource::SetName(const std::wstring& name)
	{
		m_ResourceName = name;
		if (m_Resource != nullptr && !m_ResourceName.empty())
		{
			m_Resource->SetName(m_ResourceName.c_str());
		}
	}

	std::wstring IResource::GetName() const
	{
		return m_ResourceName;
	}

	void IResource::Reset()
	{
		m_Resource.Reset();
		m_FormatSupport = {};
		m_ResourceName.clear();

		delete m_ClearValue;
		m_ClearValue = nullptr;
	}

	void IResource::CheckFeatureSupport(Microsoft::WRL::ComPtr<ID3D12Device2> device)
	{
		if (device != nullptr && m_Resource != nullptr)
		{
			auto desc = m_Resource->GetDesc();

			m_FormatSupport.Format = desc.Format;
			D3D_CHECK(device->CheckFeatureSupport(
				D3D12_FEATURE::D3D12_FEATURE_FORMAT_SUPPORT,
				&m_FormatSupport,
				sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT)));
		}
		else
		{
			m_FormatSupport.Format = {};
		}
	}
}