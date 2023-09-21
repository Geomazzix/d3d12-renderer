#pragma once
#include "Resources/IResource.h"

namespace mtd3d
{
	/// <summary>
	/// Serves as a buffer in between.
	/// </summary>
	class IBuffer : public IResource
	{
	public:
		IBuffer(const std::wstring& name = L"");
		IBuffer(Microsoft::WRL::ComPtr<ID3D12Device2> device, const D3D12_RESOURCE_DESC& desc, size_t numElements, size_t elementSize, const std::wstring& name = L"");
		virtual ~IBuffer() = default;

		virtual void CreateViews(size_t numElements, size_t elementSize) = 0;
	};
}