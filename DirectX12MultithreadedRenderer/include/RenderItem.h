#pragma once
#include <d3d12.h>
#include <memory>
#include <DirectXMath.h>

#include "Utility/MathUtility.h"
#include "Resources/Mesh.h"

using namespace DirectX;

namespace mtd3d
{
	/// <summary>
	/// Serves as a render item for the upcoming renderer class.
	/// </summary>
	struct RenderItem
	{
		UINT ObjectCBIndex = -1;
		int NumFramesDirty;

		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		
		XMFLOAT4X4 ModelMatrix = Identity();
		std::weak_ptr<Mesh> MeshInstance = std::weak_ptr<Mesh>(); //Serves as nullptr.
		std::weak_ptr<Material> MaterialInstance;

		RenderItem(int numFrames, UINT objectCBIndex, std::weak_ptr<Mesh> mesh, std::weak_ptr<Material> material, D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		{
			ObjectCBIndex = objectCBIndex;
			MeshInstance = mesh;
			NumFramesDirty = numFrames;
			PrimitiveType = primitiveType;
			MaterialInstance = material;
		}
		~RenderItem() = default;
	};
}