#pragma once
#include <memory>
#include "../Scenes/BaseScene.h"
#include <unordered_map>
#include "../Graphics Pipeline/Renderer.h"
#include "ParticleManager.h"
#include "../GameScene.h"
#include "../ThreadPool.h"

class SceneManager
{
public:
	static void Init(const int screenWidth, const int screenHeight, const float newScreenBoundsX, const float newScreenBoundsY);
	static void Close();

	static const std::shared_ptr<BaseScene>& ActiveScene()
	{
		return _activeScene;
	}

	static int GetScreenWidth() { return _screenWidth; }
	static int GetScreenHeight() { return _screenHeight; }
	static float GetScreenBoundsX() { return _screenBoundsX; }
	static float GetScreenBoundsY() { return _screenBoundsY; }

	static double GetDeltaTime() {return _deltaTime;}

	static void AddScene(const std::shared_ptr<BaseScene>& newScene, const std::string sceneName);

	static std::string GetSceneName(const std::shared_ptr<BaseScene> scene);
	static std::shared_ptr<BaseScene> GetScene(const std::string& sceneName);
	static void RemoveScene(const std::string& sceneName);
	static void ChangeScene(const std::string& sceneName, bool destroyOldScene = false, bool stopMusic = true);
	static void ResetScene(const std::string& sceneName);

	static void OnScreenResize(const int newScreenWidth, const int newScreenHeight,const float newScreenBoundsX, const float newScreenBoundsY);

	static void OnLoad();
	static void OnUpdateFrame(double deltaTime);
	static void OnRenderFrame(const std::shared_ptr<Renderer>& renderer, float dt);
	static void Shutdown();

	static void TogglePause();
	static std::shared_ptr<ThreadPool> threadPool;
private:
	static std::shared_ptr<BaseScene> _activeScene;
	static std::shared_ptr<GameScene> _LastActiveGameScene;	
	static std::unordered_map<std::string, std::shared_ptr<BaseScene>> _scenes;
	static int _screenWidth, _screenHeight;
	static float _screenBoundsX, _screenBoundsY;
	static double _deltaTime;
	static bool _paused;
		//static
public:
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator=(const SceneManager&&) = delete;
private:
	SceneManager();
	~SceneManager();

};

