#pragma once
#include <DirectXMath.h>

using namespace DirectX;

namespace mtd3d
{
	struct LightConstants
	{
		UINT DirectionalLightCount;
		UINT PointLightCount;
	};

	struct DirectionalLight
	{
		XMFLOAT3 Direction;
		float Intensity;
		XMFLOAT3 Color;
		float Padding = 0.0f;
	};

	struct PointLight
	{
		XMFLOAT3 Position;
		float Intensity;
		XMFLOAT3 Color;
		float Radius;
	};
}