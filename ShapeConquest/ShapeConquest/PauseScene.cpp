#include "PauseScene.h"
#include "Managers/InputManager.h"
#include "Managers/SceneManager.h"


PauseScene::PauseScene(const int screenWidth, const int screenHeight, const float screenBoundsX, const float screenBoundsY) : BaseScene(screenWidth, screenHeight, screenBoundsX, screenBoundsY)
{

}

PauseScene::~PauseScene()
{
}

void PauseScene::OnLoad()
{
	BaseScene::OnLoad();
}

void PauseScene::OnUpdateFrame(double deltaTime)
{
	if(InputManager::QueryButton(0,Button::A) == ButtonState::Pressed)
		SceneManager::TogglePause();
}

void PauseScene::OnRenderFrame(const std::shared_ptr<Renderer>& renderer, float dt)
{
	renderer->Render(_renderBatch,dt);
}

void PauseScene::Reset()
{
}

void PauseScene::Shutdown()
{
}

void PauseScene::OnSwitchIn()
{
}

void PauseScene::OnSwitchOut()
{
}

void PauseScene::AddGameObject(const std::shared_ptr<Object>& gameObject)
{
	BaseScene::AddGameObject(gameObject);
}

void PauseScene::RemoveGameObject(const std::shared_ptr<Object>& gameObject)
{
	BaseScene::RemoveGameObject(gameObject);
}
