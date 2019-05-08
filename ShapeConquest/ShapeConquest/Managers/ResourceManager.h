#pragma once
#include <map>
#include <memory>
#include <fstream>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

#include "../Utilities//AssetFactory.h"
#include "../Graphics Pipeline/Texture.h"
#include "../Graphics Pipeline/Geometry.h"

class ResourceManager
{
public:
	~ResourceManager();

	static std::shared_ptr<Texture> GetTexture(const wstring& textureName);
	static void AddGeometry(const std::string& refName, const std::shared_ptr<Geometry>& toStore);
	static std::shared_ptr<Geometry> GetGeometry(const std::string& geometryName);
	static std::vector<std::tuple<glm::vec3, glm::vec2, glm::vec3>> GetOBJData(const std::string& fileName);

private:
	ResourceManager();

	static std::map<const wstring, std::shared_ptr<Texture>> m_textures;
	static std::map<const std::string, std::shared_ptr<Geometry>> m_geometries;
	static std::map<const std::string, std::vector<std::tuple<glm::vec3, glm::vec2, glm::vec3>>> m_objFileData;
};

