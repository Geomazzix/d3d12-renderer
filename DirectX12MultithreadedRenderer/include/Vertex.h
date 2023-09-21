#pragma once
#include <d3d12.h>
#include <DirectXMath.h>

namespace mtd3d
{
	/// <summary>
	/// The vertex struct represents the universal vertex format used in the renderer.
	/// </summary>
	struct Vertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT4 Tangent;
		DirectX::XMFLOAT4 Color;
		DirectX::XMFLOAT2 UV;

		Vertex() = default;
		Vertex(const DirectX::XMFLOAT3& position, 
			const DirectX::XMFLOAT3& normal, 
			const DirectX::XMFLOAT4& tangent, 
			const DirectX::XMFLOAT2& uv, 
			const DirectX::XMFLOAT4& color);
		Vertex(float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz, float tw,
			float u, float v,
			float cx, float cy, float cz, float cw);
		~Vertex() = default;

		bool operator==(const Vertex& rhs) const;

		static const int s_InputElementCount = 5;
		static const D3D12_INPUT_ELEMENT_DESC s_InputElements[s_InputElementCount];
	};
}