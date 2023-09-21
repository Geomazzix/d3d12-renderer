#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <memory>

#include "Resources/UploadBuffer.h"
#include "Resources/Material.h"
#include "Light.h"
#include "Utility/D3DErrorHandler.h"

namespace mtd3d
{
	/// <summary>
	/// A frame resource holds all the data the PSO and shaders needs to get rendered on the screen.
	/// </summary>
	template<typename ObjectConstants>
	struct FrameResource
	{
		FrameResource(Microsoft::WRL::ComPtr<ID3D12Device2> device, UINT objectCount, UINT materialCount, UINT directionalLightCount, UINT pointLightCount);
		~FrameResource() = default;
		FrameResource(const FrameResource& rhs) = delete;
		FrameResource& operator=(const FrameResource& rhs) = delete;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator;
		std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;
		std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;
		std::unique_ptr<UploadBuffer<DirectionalLight>> DirectionalLightSR = nullptr;
		std::unique_ptr<UploadBuffer<PointLight>> PointLightSR = nullptr;

		UINT64 FenceValue;
	};

	template<typename ObjectConstants>
	FrameResource<ObjectConstants>::FrameResource(Microsoft::WRL::ComPtr<ID3D12Device2> device, UINT objectCount, UINT materialCount, UINT directionalLightCount, UINT pointLightCount) :
		FenceValue(0)
	{
		D3D_CHECK(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator)));
		ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
		MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, true);
		
		if (directionalLightCount > 0)
		{
			DirectionalLightSR = std::make_unique<UploadBuffer<DirectionalLight>>(device, directionalLightCount, false);
		}

		if(pointLightCount > 0)
		{
			PointLightSR = std::make_unique<UploadBuffer<PointLight>>(device, pointLightCount, false);
		}
	}
}