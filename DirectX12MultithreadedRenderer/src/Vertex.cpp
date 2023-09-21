#include "Vertex.h"

using namespace DirectX;

namespace mtd3d
{
	const D3D12_INPUT_ELEMENT_DESC Vertex::s_InputElements[] =
	{
		{ "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	bool Vertex::operator==(const Vertex& rhs) const
	{
		return  XMVector3Equal(XMLoadFloat3(&Position), XMLoadFloat3(&rhs.Position)) &&
				XMVector3Equal(XMLoadFloat3(&Normal), XMLoadFloat3(&rhs.Normal)) &&
				XMVector4Equal(XMLoadFloat4(&Tangent), XMLoadFloat4(&rhs.Tangent)) &&
				XMVector2Equal(XMLoadFloat2(&UV), XMLoadFloat2(&rhs.UV)) &&
				XMVector4Equal(XMLoadFloat4(&Color), XMLoadFloat4(&rhs.Color));
	}

	Vertex::Vertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT4& tangent, const DirectX::XMFLOAT2& uv, const DirectX::XMFLOAT4& color) :
		Position(position),
		Normal(normal),
		Tangent(tangent),
		UV(uv),
		Color(color)
	{}

	Vertex::Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty, float tz, float tw, float u, float v, float cx, float cy, float cz, float cw) :
		Position(px, py, pz),
		Normal(nx, ny, nz),
		Tangent(tx, ty, tz, tw),
		UV(u, v),
		Color(cx, cy, cz, cw)
	{}
}