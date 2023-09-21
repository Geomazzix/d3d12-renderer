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

	/// <summary>
	/// The base class for different light types, also serves per default as directional light.
	/// </summary>
	struct DirectionalLight
	{
		XMFLOAT3 Direction;
		float Intensity;
		XMFLOAT3 Color;
		float Padding = 0.0f;
	};

	/// <summary>
	/// The point light shines light in all directions defined within a radius.
	/// </summary>
	struct PointLight
	{
		XMFLOAT3 Position;
		float Intensity;
		XMFLOAT3 Color;
		float Radius;
	};
}