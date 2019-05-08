#include "LivesBar.h"
#include "LabledObject.h"
#include "../Managers/ResourceManager.h"
#include "TexturedObject.h"


LivesBar::LivesBar(std::shared_ptr<int> numLives, std::shared_ptr<int> score, std::shared_ptr<std::vector<wstring>> textureNames, glm::vec3 pos, int maxLives, float scale, glm::vec4 backingColour, const std::string& playerName)
	: _numLives(std::move(numLives)), _score(std::move(score)), _textureNames(std::move(textureNames)), _oldLives(*_numLives), _maxLives(maxLives), _oldScore(*_score), _oldTextureName(*_textureNames), _scale(scale)
{

	int pipWidth = 10;
	int pipHeight = 70;
	float widthPaddingMul = 1.2;
	float AllPipWidth = pipWidth * (_maxLives * widthPaddingMul);
	auto playerNameObj = std::make_shared<LabledObject>(glm::vec4(1, 1, 1, 1), playerName, backingColour, pos + glm::vec3(0,120,0), glm::vec3(0, 0, 0), glm::vec3(AllPipWidth * 2 + 70, 70, 1) * scale, ResourceManager::GetGeometry("Quad"), std::make_shared<RenderDescription>(ShaderEnum::Default, Camera::Types::Orthographic));
	AddObject("PlayerName", playerNameObj);
	auto scoreObj = std::make_shared<LabledObject>(glm::vec4(1, 1, 1, 1), std::to_string(*_score), backingColour, pos + glm::vec3(0, 35, 0), glm::vec3(0, 0, 0), glm::vec3(AllPipWidth *2 + 70, 70, 1) * scale, ResourceManager::GetGeometry("Quad"), std::make_shared<RenderDescription>(ShaderEnum::Default, Camera::Types::Orthographic));
	AddObject("Score", scoreObj);
	for(int i = 0; i < _maxLives; i++)
	{
		auto pip = std::make_shared<GameObject>(pos + glm::vec3(pipWidth * (i*widthPaddingMul), 80, 0), glm::vec3(0,0,0), glm::vec3(pipWidth, pipHeight, 1) * scale, glm::vec4(0.4, (1.0 / _maxLives) * i, 0,1), ResourceManager::GetGeometry("Quad"), std::make_shared<RenderDescription>(ShaderEnum::Default, Camera::Types::Orthographic));
		AddObject("Pip" + std::to_string(i), pip);
	}
	
	auto textCount = std::make_shared<LabledObject>(glm::vec4(1, 1, 1, 1), std::to_string(*_numLives), backingColour, pos + glm::vec3(AllPipWidth, 80, 0), glm::vec3(0, 0, 0), glm::vec3(70, 70, 1) * scale, ResourceManager::GetGeometry("Quad"), std::make_shared<RenderDescription>(ShaderEnum::Default, Camera::Types::Orthographic));
	AddObject("Text", textCount);
	for(unsigned int i =0; i < _oldTextureName.size();i++)
	{
		auto picBox = std::make_shared<TexturedObject>(ResourceManager::GetTexture(_oldTextureName[i].c_str()), pos + glm::vec3(AllPipWidth + 40 + (i / 2 * 55), 60 +(55 * (i % 2 == 0 ? 1 : 0)), 0), glm::vec3(0, 0, 0), glm::vec3(100, 100, 1) * scale, ResourceManager::GetGeometry("Quad"), std::make_shared<RenderDescription>(ShaderEnum::DefaultTextured, Camera::Types::Orthographic, "Default", std::vector<std::string>({ "UIDefault" })));

		AddObject("PicBox" + std::to_string(i), picBox);
	}
}


LivesBar::~LivesBar()
{
}

void LivesBar::Update(const double dt)
{
	if(*_numLives > _maxLives)
	{
		*_numLives = _maxLives;
	}
	else if (*_numLives < 0)
	{
		*_numLives = 0;
	}
	if(*_numLives != _oldLives)
	{
		auto textLable = std::dynamic_pointer_cast<LabledObject>(_keyObjects["Text"]);
		textLable->EditText(std::to_string(*_numLives));
		_oldLives = *_numLives;
		for(int i = 0; i < _maxLives; i++)
		{
			if(i < _oldLives)
			{
				_keyObjects["Pip" + std::to_string(i)]->alive(true);
			}
			else
			{
				_keyObjects["Pip" + std::to_string(i)]->alive(false);
			}
		}
	}
	if(*_score != _oldScore)
	{
		auto textLable = std::dynamic_pointer_cast<LabledObject>(_keyObjects["Score"]);
		textLable->EditText(std::to_string(*_score));
		_oldScore = *_score;
	}
	for(int i =0; i < _oldTextureName.size(); i++)
	{
		auto name = (*_textureNames)[i];
		if(name != _oldTextureName[i])
		{
			auto textureObj = std::dynamic_pointer_cast<TexturedObject>(_keyObjects["PicBox" + std::to_string(i)]);
			textureObj->SetTexture(ResourceManager::GetTexture(name.c_str()));
			_oldTextureName[i] = name;
		}
	}
}

void LivesBar::Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera)
{
}
