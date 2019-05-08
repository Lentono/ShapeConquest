#include "GameOverScene.h"
#include "../Managers/ResourceManager.h"
#include "../GameObjects/LabledObject.h"
#include "../Managers/InputManager.h"
#include "../GameObjects/ButtonObject.h"
#include "../Managers/SceneManager.h"


GameOverScene::GameOverScene(int screenWidth, int screenHeight, const float screenBoundsX, const float screenBoundsY, int highScore, int winningPlayerNumber
) : BaseScene(screenWidth, screenHeight, screenBoundsX, screenBoundsY), _winningScore(highScore), _winningPlayerNumber(winningPlayerNumber)
{
}


GameOverScene::~GameOverScene()
{
}

void GameOverScene::OnUpdateFrame(double deltaTime)
{
	for (const auto& object : _objects)
	{
		object->Update(deltaTime);
	}
}

void GameOverScene::OnRenderFrame(const std::shared_ptr<Renderer>& renderer, float dt)
{
	renderer->Render(_renderBatch, dt);
}

void GameOverScene::Reset()
{
}

void GameOverScene::Shutdown()
{
}

void GameOverScene::OnSwitchIn()
{
	InputManager::InitializeXInputGamePads();
}

void GameOverScene::OnSwitchOut()
{
}

void GameOverScene::OnLoad()
{
	float ScreenSizeScaler = (_screenWidth / 800.0);
	auto splashText = std::make_shared<LabledObject>(glm::vec4(0.21f, 0.1f, 0.38f, 0.5f), "Game Over!", glm::vec4(0, 0, 0, 0), glm::vec3((_screenWidth * 0.23), _screenHeight * 0.9f, -1), glm::vec3(0, 0, 0), glm::vec3(ScreenSizeScaler * 500, ScreenSizeScaler * 110, 1), ResourceManager::GetGeometry("Quad"), std::make_shared<RenderDescription>(ShaderEnum::Default, Camera::Types::Orthographic));
	AddGameObject(splashText);
	if (_winningScore > 0)
	{
		splashText = std::make_shared<LabledObject>(glm::vec4(0.21f, 0.1f, 0.38f, 0.5f), "Player " + std::to_string(_winningPlayerNumber) + " Won", glm::vec4(0, 0, 0, 0), glm::vec3((_screenWidth * 0.33), _screenHeight * 0.7f, -1), glm::vec3(0, 0, 0), glm::vec3(ScreenSizeScaler * 500, ScreenSizeScaler * 70, 1), ResourceManager::GetGeometry("Quad"), std::make_shared<RenderDescription>(ShaderEnum::Default, Camera::Types::Orthographic));
		AddGameObject(splashText);
		splashText = std::make_shared<LabledObject>(glm::vec4(0.21f, 0.1f, 0.38f, 0.5f), "Scoring " + std::to_string(_winningScore) + " Points", glm::vec4(0, 0, 0, 0), glm::vec3((_screenWidth * 0.3), _screenHeight * 0.55f, -1), glm::vec3(0, 0, 0), glm::vec3(ScreenSizeScaler * 500, ScreenSizeScaler * 70, 1), ResourceManager::GetGeometry("Quad"), std::make_shared<RenderDescription>(ShaderEnum::Default, Camera::Types::Orthographic));
		AddGameObject(splashText);
	}
	else
	{
		splashText = std::make_shared<LabledObject>(glm::vec4(0.21f, 0.1f, 0.38f, 0.5f), "Nobody Scored Any Points", glm::vec4(0, 0, 0, 0), glm::vec3((_screenWidth * 0.15), _screenHeight * 0.55f, -1), glm::vec3(0, 0, 0), glm::vec3(ScreenSizeScaler * 500, ScreenSizeScaler * 70, 1), ResourceManager::GetGeometry("Quad"), std::make_shared<RenderDescription>(ShaderEnum::Default, Camera::Types::Orthographic));
		AddGameObject(splashText);
	}

	glm::vec3 buttonScale = glm::vec3(135 * ScreenSizeScaler, 75 * ScreenSizeScaler, 1);
	auto doneButton = std::make_shared<ButtonObject>(true, glm::vec4(1, 1, 1, 1), glm::vec4(0.2, 0.2, 0.3, 0.7), "Done", glm::vec3((_screenWidth * 0.5) - (buttonScale.x / 2), _screenHeight * 0.2, -1), glm::vec3(0, 0, 0), buttonScale, ResourceManager::GetGeometry("Quad"), std::make_shared<RenderDescription>(ShaderEnum::Default, Camera::Types::Orthographic));
	
	doneButton->SetOnClick([]()
	{
		SceneManager::ChangeScene("MainMenu", true);
	});
	AddGameObject(doneButton);
}
