#pragma once
#include <memory>
#include "../Graphics Pipeline/Graphics.h"
#include "../Graphics Pipeline/Renderer.h"
#include <unordered_map>

class BaseScene
{
public:
	BaseScene(const int screenWidth, const int screenHeight, const float screenBoundsX, const float screenBoundsY);
	virtual ~BaseScene();

	virtual void OnLoad();
	virtual void OnUpdateFrame(double deltaTime) = 0;
	void UpdateScreenSize(const int newScreenWidth, const int newScreenHeight,const int newScreenBoundsX, const int newScreenBoundsY);
	virtual void OnRenderFrame(const std::shared_ptr<Renderer>& renderer, float dt) = 0;
	

	virtual void AddGameObject(const std::shared_ptr<Object>& gameObject);
	virtual void RemoveGameObject(const std::shared_ptr<Object>& gameObject);
	virtual void Reset() = 0;
	virtual void Shutdown() = 0;
	virtual void OnSwitchIn() = 0; // called when set to current scene
	virtual void OnSwitchOut() = 0; // called when set to no longer be the current scene

	bool Loaded() const { return _Loaded;}
	void Loaded(const bool loaded) { _Loaded = loaded;}

	const std::unordered_map<std::string, std::vector<std::shared_ptr<GameObject>>>& GetRenderBatch() const { return _renderBatch; } // game objects of scene organized by render targets

protected:
	int _screenWidth, _screenHeight = 0;
	float _screenBoundsX, _screenBoundsY = 0;
	std::vector<std::shared_ptr<Object>> _objects;
	std::unordered_map<std::string, std::vector<std::shared_ptr<GameObject>>> _renderBatch;//Batch of objects to be rendered to non default targets
private:
	bool _Loaded = false;

};

