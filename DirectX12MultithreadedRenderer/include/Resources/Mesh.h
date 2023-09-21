#pragma once

#include "Resources/VertexBuffer.h"
#include "Resources/IndexBuffer.h"

#include "CommandList.h"
#include "Vertex.h"

#include <d3d12.h>
#include <wrl.h>
#include <memory>

namespace mtd3d
{
	/// <summary>
	/// The mesh holds owner ship over vertex/index data buffers.
	/// </summary>
	class Mesh
	{
	public:
		Mesh() = default;
		~Mesh() = default;

		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device2> device,
			std::weak_ptr<CommandList> commandList,
			const std::vector<Vertex>& vertices,
			const std::vector<uint16_t>& indicies);
		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device2> device,
			std::weak_ptr<CommandList> commandList,
			const std::vector<Vertex>& vertices,
			const std::vector<uint32_t>& indicies);

		size_t GetIndexCount() const;
		void RecordDrawCommand(std::weak_ptr<CommandList> commandList, uint32_t instanceCount, uint32_t firstInstance);

	private:
		VertexBuffer m_VertexBuffer;
		IndexBuffer m_IndexBuffer;
	};
}