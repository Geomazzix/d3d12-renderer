#pragma once
#include "Resources/Mesh.h"
#include "RenderDevice.h"
#include "Vertex.h"

#include <memory>
#include <DirectXMath.h>
#include <vector>

namespace mtd3d
{
	/// <summary>
	/// Some utility for mesh data handling/loading.
	/// </summary>
	class MeshUtility
	{
	public:
		static std::shared_ptr<Mesh> CreateGeoSphere(std::weak_ptr<RenderDevice> device, float radius, uint32_t numSubdivisions, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f));
		static std::shared_ptr<Mesh> CreateCylinder(std::weak_ptr<RenderDevice> device, float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f));
		static std::shared_ptr<Mesh> CreateSphere(std::weak_ptr<RenderDevice> device, float radius, uint32_t sliceCount, uint32_t stackCount, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f));
		static std::shared_ptr<Mesh> CreatePlane(std::weak_ptr<RenderDevice> device, float cellWidth, float cellLength, uint32_t columnCount, uint32_t rowCount, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f));
		static std::shared_ptr<Mesh> CreateQuad(std::weak_ptr<RenderDevice> device, float x, float y, float w, float h, float depth, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f));
		static std::shared_ptr<Mesh> CreateBox(std::weak_ptr<RenderDevice> device, float width, float height, float depth, uint32_t numSubdivisions, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f));
	
	private:
		struct MeshData
		{
			std::vector<Vertex> Vertices;
			std::vector<uint32_t> Indices32;

			std::vector<uint16_t>& GetIndices16()
			{
				if (m_Indices16.empty())
				{
					m_Indices16.resize(Indices32.size());
					for (size_t i = 0; i < Indices32.size(); ++i)
						m_Indices16[i] = static_cast<uint16_t>(Indices32[i]);
				}

				return m_Indices16;
			}

		private:
			std::vector<uint16_t> m_Indices16;
		};

		static void Subdivide(MeshData& meshData);
		static Vertex MidPoint(const Vertex& v0, const Vertex& v1);
	};
}