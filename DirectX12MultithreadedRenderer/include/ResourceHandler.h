#pragma once

#include "Resources/Mesh.h"
#include "Resources/Material.h"

#include <unordered_map>
#include <string>
#include <memory>

namespace mtd3d
{
	/// <summary>
	/// The resource handler takes care of the loading of objects from an higher layer and caches them.
	/// </summary>
	class ResourceHandler
	{
	public:
		ResourceHandler() = default;
		~ResourceHandler() = default;
	
		void CacheMesh(const std::string& key, std::shared_ptr<Mesh> mesh);
		std::weak_ptr<Mesh> GetMesh(const std::string& key);
		void UnloadMesh(const std::string& key);
		size_t GetMeshCount() const;

		void CacheMaterial(const std::string& key, std::shared_ptr<Material> material);
		std::weak_ptr<Material> GetMaterial(const std::string& key);
		void UnloadMaterial(const std::string& key);
		size_t GetMaterialCount() const;

		void UnloadResources();

	private:
		std::unordered_map<std::string, std::shared_ptr<Mesh>> m_MeshBuffer;
		std::unordered_map<std::string, std::shared_ptr<Material>> m_MaterialBuffer;
	};
}