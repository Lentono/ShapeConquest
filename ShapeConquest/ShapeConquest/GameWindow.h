#pragma once
#include <memory>
#include "Graphics Pipeline/Renderer.h"
#include "Managers/SceneManager.h"

class GameWindow
{
public:
	GameWindow();
	~GameWindow();

	void Run();
	void OnLoad();
	void OnScreenResize() const;
	void OnUpdateFrame(double deltaTime) const;
	void OnRenderFrame(float dt) const;
	void Shutdown() const;

	std::shared_ptr<Renderer> _renderer;
	//static std::shared_ptr<SceneManager> _SceneManager;

	int _ScreenHeight = 1080;
	int _ScreenWidth = 1920;
	int _screenBoundsX = 0;
	int _screenBoundsY = 0;
	std::string _ApplicationName = "Shape Conquest";
};

