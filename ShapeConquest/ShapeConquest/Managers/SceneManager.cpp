#include "SceneManager.h"
#include "../AudioManager.h"

std::shared_ptr<BaseScene> SceneManager::_activeScene;
std::shared_ptr<GameScene> SceneManager::_LastActiveGameScene;
std::unordered_map<std::string, std::shared_ptr<BaseScene>> SceneManager::_scenes;
int SceneManager::_screenWidth, SceneManager::_screenHeight;
float SceneManager::_screenBoundsX, SceneManager::_screenBoundsY;
double SceneManager::_deltaTime;
bool SceneManager::_paused;
std::shared_ptr<ThreadPool> SceneManager::threadPool;
void SceneManager::Init(const int screenWidth, const int screenHeight, const float screenBoundsX, const float screenBoundsY)
{
	_screenWidth = screenWidth;
	_screenHeight = screenHeight;
	_screenBoundsX = screenBoundsX;
	_screenBoundsY = screenBoundsY;
	threadPool = std::make_shared<ThreadPool>(30);
}

void SceneManager::AddScene(const std::shared_ptr<BaseScene>& newScene, const std::string sceneName)
{
	_scenes.emplace(sceneName, newScene);
}

std::string SceneManager::GetSceneName(const std::shared_ptr<BaseScene> scene)
{
	auto itr = _scenes.begin();

	while (itr != _scenes.end())
	{
		if (itr->second == scene)
			return itr->first;
		++itr;
	}
	return	"";
}

std::shared_ptr<BaseScene> SceneManager::GetScene(const std::string& sceneName)
{
	const auto itr = _scenes.find(sceneName);
	if (itr == _scenes.end())
		return nullptr; // No scene with sceneName found
	return itr->second; // Return handle to scene
}

void SceneManager::RemoveScene(const std::string& sceneName)
{
	_scenes.erase(sceneName); // Remove Scene with sceneName
}

void SceneManager::ChangeScene(const std::string& sceneName, bool destroyOldScene, bool stopMusic)
{
	if (stopMusic)
		AudioManager::StopSounds();
	if (_activeScene)
	{
		_activeScene->OnSwitchOut();
	}
	if(destroyOldScene)
	{
		for(auto i = _scenes.begin(); i != _scenes.end();++i)
		{
			if(i->second == _activeScene)
			{
				_scenes.erase(i);
				break;
			}
		}
	}
	_activeScene = GetScene(sceneName); // Sets active scene to scene with sceneName
	if (!_activeScene->Loaded())
	{
		_activeScene->OnLoad();
	}
	_activeScene->OnSwitchIn();

}

void SceneManager::ResetScene(const std::string& sceneName)
{
	GetScene(sceneName)->Reset(); // Reset scene
}

void SceneManager::OnScreenResize(const int newScreenWidth, const int newScreenHeight, const float newScreenBoundsX, const float newScreenBoundsY)
{
	for (auto &i : _scenes)
		i.second->UpdateScreenSize(newScreenWidth, newScreenHeight, newScreenBoundsX, newScreenBoundsY);
}

void SceneManager::OnLoad()
{
	_activeScene->OnLoad(); // Load active scene
}

void SceneManager::OnUpdateFrame(double deltaTime)
{
	_deltaTime = deltaTime;

	_activeScene->OnUpdateFrame(deltaTime); // Update active scene
}

void SceneManager::OnRenderFrame(const std::shared_ptr<Renderer>& renderer, float dt)
{
	_activeScene->OnRenderFrame(renderer, dt); // Render active scene
}

void SceneManager::Shutdown()
{
	// Shutdown all scenes
	for (auto &i : _scenes)
		i.second->Shutdown();
	// Remove handles to scenes
	_scenes.clear();
	// Null active scene
	_activeScene = nullptr;
	AudioManager::StopSounds();
}

void SceneManager::TogglePause()
{
	if (_paused)
	{
		_paused = false;
		const auto previousSceneName = GetSceneName(_LastActiveGameScene);
		ChangeScene(previousSceneName, false);
	}
	else
	{
		_paused = true;
		if (const auto gameScene = std::dynamic_pointer_cast<GameScene>(_activeScene))
			_LastActiveGameScene = gameScene;
		ChangeScene("PauseScene", false);
	}

}


//Static
SceneManager::SceneManager()
= default;

SceneManager::~SceneManager()
= default;