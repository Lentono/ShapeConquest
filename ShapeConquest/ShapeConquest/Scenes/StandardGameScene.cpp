#include "StandardGameScene.h"
#include "../Bullet.h"
#include "../Managers/SceneManager.h"
#include "../GravityBomb.h"
#include <ctime>
#include "../GlobalStructures.h"
#include "../Managers/ParticleManager.h"
#include "../AudioManager.h"


StandardGameScene::StandardGameScene(const int screenWidth, const int screenHeight, const float screenBoundsX, const float screenBoundsY) : GameScene(screenWidth, screenHeight, screenBoundsX, screenBoundsY)
{
}

StandardGameScene::~StandardGameScene()
{
	Shutdown();
}

void StandardGameScene::OnLoad()
{
	GameScene::OnLoad();
	SpawnPowerUp();
	_DefaultAISpawnFrequency = _AISpawnFrequency;
	/*
	AddGameObject(std::make_shared<AI>(true, 0.1, glm::vec3(2, 0, 0), glm::angleAxis(glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(0.1, 0.1, 0.1), glm::vec4(0.0, 1.0, 0.0, 1.0), AssetFactory::LoadGeometry_VTN("Assets/Models/BensDankLeaf.obj"), std::make_shared<RenderDescription>(ShaderEnum::Default), Shuriken));
	AddGameObject(std::make_shared<AI>(true, 0.1, glm::vec3(-2, 0, 0), glm::quat(0, 0, 0, 1.0), glm::vec3(0.1, 0.1, 0.1), glm::vec4(0.0, 0.0, 1.0, 1.0), AssetFactory::LoadGeometry_VTN("Assets/Models/DodecahedronFour.obj"), std::make_shared<RenderDescription>(ShaderEnum::Default), Shrapnel));
	AddGameObject(std::make_shared<AI>(true, 0.1, glm::vec3(2, 2, 0), glm::quat(0, 0, 0, 1.0), glm::vec3(0.1, 0.1, 0.1), glm::vec4(0.5, 0.5, 0.5, 1.0), AssetFactory::LoadGeometry_VTN("Assets/Models/DodecahedronFour.obj"), std::make_shared<RenderDescription>(ShaderEnum::Default), Shrapnel));
	AddGameObject(std::make_shared<AI>(true, 0.1, glm::vec3(-2, -2, 0), glm::quat(0, 0, 0, 1.0), glm::vec3(0.1, 0.1, 0.1), glm::vec4(0.2, 0.3, 0.4, 1.0), AssetFactory::LoadGeometry_VTN("Assets/Models/DodecahedronFour.obj"), std::make_shared<RenderDescription>(ShaderEnum::Default), Shrapnel));
	AddGameObject(std::make_shared<AI>(true, 0.1, glm::vec3(0, 0, 0), glm::quat(0, 0, 0, 1.0), glm::vec3(0.1, 0.1, 0.1), glm::vec4(0.5, 0.4, 0.7, 1.0), AssetFactory::LoadGeometry_VTN("Assets/Models/DodecahedronFour.obj"), std::make_shared<RenderDescription>(ShaderEnum::Default), Shrapnel));*/

	//std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene())->GetParticleSystemManager()->AddParticleSystem(EmitterType::Cone, 1.0f, glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene())->GetParticleSystemManager()->AddParticleSystem(EmitterType::Cone, 20.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene())->GetParticleSystemManager()->AddParticleSystem(EmitterType::Cone, 1.0f, glm::vec3(-2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//AddParticleSystem(EmitterType::Cone, 1.0f, glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void StandardGameScene::OnUpdateFrame(double deltaTime)
{
	GameScene::OnUpdateFrame(deltaTime);

	if ((_players.size() / 10.0f) < 1.0f)
		_DefaultAISpawnFrequency = 0.5* (1.0f - (_players.size() / 10.0f));
	if (_TimeSincePowerUpSpawn > _PowerUpSpawnFrequency)
	{
		_TimeSincePowerUpSpawn = 0.0f;
		SpawnPowerUp();
	}
	else
		_TimeSincePowerUpSpawn += deltaTime;

	if (_TimeSinceAISpawn > _AISpawnFrequency)
	{
		_TimeSinceAISpawn = 0.0f;
		SpawnAI();
	}
	else
		_TimeSinceAISpawn += deltaTime;

	if ((_difficultLevel / 100.0f) < 1.0f)
		_AISpawnFrequency = _DefaultAISpawnFrequency * (1.0f - (_difficultLevel / 100.0f));

}

void StandardGameScene::OnRenderFrame(const std::shared_ptr<Renderer>& renderer, float dt)
{
	GameScene::OnRenderFrame(renderer, dt);
}

void StandardGameScene::Reset()
{
	// TODO: Add logic to reset the standard game scene here
	GameScene::Reset();
}

void StandardGameScene::Shutdown()
{
	// TODO: Ensure all scene objects are correctly shutdown
	GameScene::Shutdown();
}

void StandardGameScene::AddGameObject(const std::shared_ptr<Object>& gameObject)
{
	GameScene::AddGameObject(gameObject);
}

void StandardGameScene::RemoveGameObject(const std::shared_ptr<Object>& gameObject)
{
	GameScene::RemoveGameObject(gameObject);

	//TODO: Implement any checks on collections that are unique to this scene
}

void StandardGameScene::OnSwitchIn()
{
	GameScene::OnSwitchIn();
	SceneManager::threadPool->AddWork([]() {AudioManager::PlaySound("Assets/Audio/MainSoundtrack.wav", true); });
}

void StandardGameScene::OnSwitchOut()
{
	GameScene::OnSwitchOut();
}
