#include "Resources/Mesh.h"

using namespace Microsoft::WRL;
using namespace DirectX;

namespace mtd3d
{
	void Mesh::Initialize(
		ComPtr<ID3D12Device2> device, 
		std::weak_ptr<CommandList> commandList, 
		const std::vector<Vertex>& vertices, 
		const std::vector<uint16_t>& indicies)
	{
		assert(vertices.size() < USHORT_MAX && "Too many vertices for 16-bit index buffer!");

		m_VertexBuffer.SetName(L"VertexBuffer");
		commandList.lock()->CopyVertexBuffer(device, m_VertexBuffer, vertices);

		m_IndexBuffer.SetName(L"IndexBuffer");
		commandList.lock()->CopyIndexBuffer(device, m_IndexBuffer, indicies);
	}

	void Mesh::Initialize(
		ComPtr<ID3D12Device2> device, 
		std::weak_ptr<CommandList> commandList, 
		const std::vector<Vertex>& vertices, 
		const std::vector<uint32_t>& indicies)
	{
		assert(vertices.size() < UINT_MAX && "Too many vertices for 16-bit index buffer!");

		m_VertexBuffer.SetName(L"VertexBuffer");
		commandList.lock()->CopyVertexBuffer(device, m_VertexBuffer, vertices);

		m_IndexBuffer.SetName(L"IndexBuffer");
		commandList.lock()->CopyIndexBuffer(device, m_IndexBuffer, indicies);
	}

	size_t Mesh::GetIndexCount() const
	{
		return m_IndexBuffer.GetNumIndicies();
	}

	void Mesh::RecordDrawCommand(std::weak_ptr<CommandList> commandList, uint32_t instanceCount, uint32_t firstInstance)
	{
		if(auto cl = commandList.lock())
		{
			cl->SetVertexBuffer(0, m_VertexBuffer);
			size_t indexCount = m_IndexBuffer.GetNumIndicies();

			if (indexCount > 0)
			{
				cl->SetIndexBuffer(m_IndexBuffer);
				cl->RecordIndexedDraw(static_cast<uint32_t>(indexCount), instanceCount, 0, 0, firstInstance);
			}
			else
			{
				cl->RecordDraw(static_cast<uint32_t>(m_VertexBuffer.GetVertexCount()), instanceCount, 0, firstInstance);
			}
		}
	}
}