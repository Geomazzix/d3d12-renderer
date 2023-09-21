#pragma once
#include <DirectXMath.h>

using namespace DirectX;

namespace mtd3d
{
	struct MaterialConstants
	{
		XMFLOAT4 AlbedoDiffuse = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
		XMFLOAT3 FresnelR0 = XMFLOAT3(0.04f, 0.04f, 0.04f);
		UINT RoughnessFactor = 0u;
	};


	/// <summary>
	/// Defines the default material to be used in the shader.
	/// </summary>
	struct Material
	{
		int CbIndex = -1;
		int NumFramesDirty;

		MaterialConstants Constants;

		Material() = default;
		Material(int numFrames, int cbIndex) : NumFramesDirty(0), CbIndex(-1), Constants({})
		{
			CbIndex = cbIndex;
			NumFramesDirty = numFrames;
		}
		~Material() = default;
	};
}