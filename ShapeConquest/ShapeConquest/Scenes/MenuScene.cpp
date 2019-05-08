#include "MenuScene.h"
#include "../GameObjects/LabledObject.h"
#include "../GameObjects/ButtonObject.h"
#include "../Managers/SceneManager.h"
#include "../Managers/ResourceManager.h"
#include "../GameObjects/LivesBar.h"
#include "../AudioManager.h"
#include "StandardGameScene.h"


MenuScene::MenuScene(int screenWidth, int screenHeight, const float screenBoundsX, const float screenBoundsY) : BaseScene(screenWidth, screenHeight, screenBoundsX,screenBoundsY)
{
}


MenuScene::~MenuScene()
= default;

void MenuScene::OnLoad()
{

	float ScreenSizeScaler = (_screenWidth / 800.0);
	glm::vec3 buttonScale = glm::vec3(200 * ScreenSizeScaler, 75 * ScreenSizeScaler, 1);
	auto startButton = std::make_shared<ButtonObject>(true, glm::vec4( 1,1,1,1 ), glm::vec4(0.2, 0.2, 0.3, 0.7),"Start", glm::vec3(10 * ScreenSizeScaler, _screenHeight * 0.7, -1), glm::vec3(0, 0, 0), buttonScale, ResourceManager::GetGeometry("Quad"), std::make_shared<RenderDescription>(ShaderEnum::Default, Camera::Types::Orthographic));
	auto exitButton = std::make_shared<ButtonObject>(false, glm::vec4(0, 0, 0, 1), glm::vec4(0.2, 0.2, 0.3, 0.7), "Quit", glm::vec3(10 * ScreenSizeScaler, _screenHeight * 0.4, -1), glm::vec3(0, 0, 0), buttonScale, ResourceManager::GetGeometry("Quad"), std::make_shared<RenderDescription>(ShaderEnum::Default, Camera::Types::Orthographic));
	startButton->AddNeighbor(Down, exitButton);
	startButton->SetOnClick([]()
	{
		SceneManager::AddScene(std::make_shared<StandardGameScene>(SceneManager::GetScreenWidth(), SceneManager::GetScreenHeight(), SceneManager::GetScreenBoundsX(), SceneManager::GetScreenBoundsY()), "GameScene");
		SceneManager::ChangeScene("GameScene");
	});
	exitButton->AddNeighbor(Up, startButton);
	exitButton->SetOnClick([]()
	{
		PostQuitMessage(0);
	});
	AddGameObject(startButton);
	AddGameObject(exitButton);

	auto splashText = std::make_shared<LabledObject>(glm::vec4(0.21f, 0.1f, 0.38f, 0.5f), "SHAPE CONQUEST", glm::vec4(0, 0, 0, 0), glm::vec3((_screenWidth * 0.3), _screenHeight * 0.9f, -1), glm::vec3(0, 0, 0), glm::vec3(ScreenSizeScaler * 500, ScreenSizeScaler * 90, 1), ResourceManager::GetGeometry("Quad"), std::make_shared<RenderDescription>(ShaderEnum::Default, Camera::Types::Orthographic));
	AddGameObject(splashText);

	_bigBoss = std::make_shared<GameObject>(glm::vec3(5, -5, -20), glm::vec3(0, 0, 0), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec4(((rand() % 100) / 100.0), ((rand() % 100) / 100.0), ((rand() % 100) / 100.0),0.5), ResourceManager::GetGeometry("BigBoss.obj"), std::make_shared<RenderDescription>());
	AddGameObject(_bigBoss);

	BaseScene::OnLoad();
}

void MenuScene::OnUpdateFrame(double deltaTime)
{
	_bigBoss->setRotation(_bigBoss->getRotation() * glm::quat(1, deltaTime * ((rand()%100) / 100.0), deltaTime* ((rand() % 100) / 100.0), deltaTime* ((rand() % 100) / 100.0)));
	for(const auto& object : _objects)
	{
		object->Update(deltaTime);
	}
}

void MenuScene::OnRenderFrame(const std::shared_ptr<Renderer>& renderer, float dt)
{
	renderer->Render(_renderBatch, dt);
}

void MenuScene::Reset()
{

}

void MenuScene::Shutdown()
{
	
}

void MenuScene::OnSwitchIn()
{
	InputManager::InitializeXInputGamePads();

	AudioManager::PlaySound("Assets/Audio/ShapeConquestSoundtrack.wav", true);
}

void MenuScene::OnSwitchOut()
{
}
