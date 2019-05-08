#include "ResourceManager.h"

std::map<const wstring, std::shared_ptr<Texture>> ResourceManager::m_textures;
std::map<const std::string, std::shared_ptr<Geometry>> ResourceManager::m_geometries;
std::map<const std::string, std::vector<std::tuple<glm::vec3, glm::vec2, glm::vec3>>> ResourceManager::m_objFileData;

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

std::shared_ptr<Texture> ResourceManager::GetTexture(const wstring& textureName)
{
	const std::wstring textureFolder = L"Assets/Textures/";
	std::wstring texturePath = textureFolder + textureName;

	if (m_textures.count(textureName) == 0)
	{
		m_textures.insert(std::make_pair(textureName, AssetFactory::LoadTexture(texturePath.c_str())));
	}

	return m_textures.at(textureName);
}

void ResourceManager::AddGeometry(const std::string& refName, const std::shared_ptr<Geometry>& toStore)
{
	const auto found = m_geometries.find(refName);
	if(found != m_geometries.end())
	{
		OutputHandler::ErrorOut("Tried to add a geometry with the same name as a previously added geometry. STOP IT, YOU CAD!", __LINE__, __FILE__);
		return;
	}
	m_geometries[refName] = toStore;
}

std::shared_ptr<Geometry> ResourceManager::GetGeometry(const std::string& geometryName)
{
	if (m_geometries.count(geometryName) == 0)
	{
		m_geometries.insert(std::make_pair(geometryName, AssetFactory::LoadGeometry_VTN("Assets/Models/" + geometryName)));
	}

	return m_geometries.at(geometryName);
}

std::vector<std::tuple<glm::vec3, glm::vec2, glm::vec3>> ResourceManager::GetOBJData(const std::string& fileName)
{
	if (m_objFileData.count(fileName) == 0)
	{
		m_objFileData.insert(std::make_pair(fileName, AssetFactory::LoadObjData_VTN("Assets/Models/" + fileName)));
	}

	return m_objFileData.at(fileName);
}