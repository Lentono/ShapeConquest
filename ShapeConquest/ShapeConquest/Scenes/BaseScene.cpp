#include "BaseScene.h"



BaseScene::BaseScene(const int screenWidth, const int screenHeight, const float screenBoundsX, const float screenBoundsY) : _screenWidth(screenWidth), _screenHeight(screenHeight),
_screenBoundsX(screenBoundsX), _screenBoundsY(screenBoundsY)
{
}


BaseScene::~BaseScene()
{
}

void BaseScene::OnLoad()
{
	_Loaded = true;
}

void BaseScene::UpdateScreenSize(const int newScreenWidth, const int newScreenHeight,const int newScreenBoundsX, const int newScreenBoundsY)
{
	_screenWidth = newScreenWidth;
	_screenHeight = newScreenHeight;
	_screenBoundsX = newScreenBoundsX;
	_screenBoundsY = newScreenBoundsY;
}

void BaseScene::AddGameObject(const std::shared_ptr<Object>& object)
{
	_objects.push_back(object);
	auto gameObject = dynamic_pointer_cast<GameObject>(object);
	if (gameObject)
	{
		auto targets = gameObject->RenderTargets();

		//add gameObject to all target render batches
		for (const auto& t : targets)
		{
			_renderBatch[t].push_back(gameObject);
		}
	}
}

void BaseScene::RemoveGameObject(const std::shared_ptr<Object>& object)
{
	auto gameObject = dynamic_pointer_cast<GameObject>(object);
	if (gameObject)
	{
		auto targets = gameObject->RenderTargets();

		//deletes gameObject from all render batches
		for (const auto& t : targets)
		{
			for (unsigned int i = 0; i < _renderBatch[t].size(); i++)
			{
				if (_renderBatch[t][i] == gameObject)
				{
					_renderBatch[t].erase(_renderBatch[t].begin() + i);
					break; // assumed to only be one reference to the object in this vector, 
				}
			}
		}
	}
	//deletes target from 
	for(unsigned int i = 0; i < _objects.size(); i++)
	{
		if(_objects[i] == gameObject)
		{
			_objects.erase(_objects.begin() + i);
			break; // assumed to only be one reference to the object in this vector, 
		}
	}
}


