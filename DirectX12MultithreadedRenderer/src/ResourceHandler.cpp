#include "ResourceHandler.h"

namespace mtd3d
{
	void ResourceHandler::CacheMesh(const std::string& key, std::shared_ptr<Mesh> mesh)
	{
		if(m_MeshBuffer.find(key) != m_MeshBuffer.end())
		{
			return;
		}

		m_MeshBuffer.emplace(key, mesh);
	}

	std::weak_ptr<mtd3d::Mesh> ResourceHandler::GetMesh(const std::string& key)
	{
		if (m_MeshBuffer.find(key) != m_MeshBuffer.end())
		{
			return m_MeshBuffer[key];
		}

		printf("ERROR: %s is not yet loaded in.\n", key.c_str());
		return std::make_shared<Mesh>();
	}

	void ResourceHandler::UnloadMesh(const std::string& key)
	{
		if (m_MeshBuffer.find(key) != m_MeshBuffer.end())
		{
			m_MeshBuffer.erase(key);
			return;
		}

		printf("ERROR: The %s, can not be unloaded.\n", key.c_str());
	}

	size_t ResourceHandler::GetMeshCount() const
	{
		return m_MeshBuffer.size();
	}

	void ResourceHandler::CacheMaterial(const std::string& key, std::shared_ptr<Material> material)
	{
		if (m_MaterialBuffer.find(key) != m_MaterialBuffer.end())
		{
			return;
		}

		m_MaterialBuffer.emplace(key, material);
	}

	std::weak_ptr<mtd3d::Material> ResourceHandler::GetMaterial(const std::string& key)
	{
		if (m_MaterialBuffer.find(key) != m_MaterialBuffer.end())
		{
			return m_MaterialBuffer[key];
		}

		printf("ERROR: %s is not yet loaded in.\n", key.c_str());
		return std::make_shared<Material>();
	}

	void ResourceHandler::UnloadMaterial(const std::string& key)
	{
		if (m_MaterialBuffer.find(key) != m_MaterialBuffer.end())
		{
			m_MaterialBuffer.erase(key);
			return;
		}

		printf("ERROR: The %s, can not be unloaded.\n", key.c_str());
	}

	size_t ResourceHandler::GetMaterialCount() const
	{
		return m_MaterialBuffer.size();
	}

	void ResourceHandler::UnloadResources()
	{
		m_MeshBuffer.clear();
	}
}