#include "Resources/IBuffer.h"

namespace mtd3d
{
	IBuffer::IBuffer(const std::wstring& name /*= L""*/) :
		IResource(name)
	{}

	IBuffer::IBuffer(Microsoft::WRL::ComPtr<ID3D12Device2> device, const D3D12_RESOURCE_DESC& desc, size_t numElements, size_t elementSize, const std::wstring& name /*= L""*/) :
		IResource(device, desc, nullptr, name)
	{}
}