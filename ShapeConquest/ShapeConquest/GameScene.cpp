#include "GameScene.h"
#include "GameObjects/AI.h"
#include "GameObjects/PlayerZone.h"
#include <ctime>
#include "GravityBomb.h"
#include "Utilities/CollisionHandler.h"

#include "Managers/SceneManager.h"
#include "AudioManager.h"

#include "GameObjects/LivesBar.h"
#include "Scenes/GameOverScene.h"


GameScene::GameScene(const int screenWidth, const int screenHeight, const float screenBoundsX, const float screenBoundsY) : BaseScene(screenWidth, screenHeight, screenBoundsX, screenBoundsY), m_particleSystemManager(std::make_shared<ParticleManager>())
{

}


GameScene::~GameScene()
{
}

void GameScene::OnLoad()
{
	BaseScene::OnLoad();

	AddGameObject(m_particleSystemManager);

	GlobalStructures::ScreenBoundsX = _screenBoundsX;
	GlobalStructures::ScreenBoundsY = _screenBoundsY;
	GlobalStructures::SpawnGridCellSize = _spawnGridCellSize;
	GlobalStructures::CreateSpawnGrid();

	_QuadTree = std::make_shared<BoxCollider>(_screenBoundsX * 2, _screenBoundsY * 2, glm::vec3(0, 0, 0), 0.5f);

	_dificultyUI = std::make_shared<LabledObject>(glm::vec4(1,1,1, 0.5f), "0", glm::vec4(0, 0, 0, 0.5), glm::vec3((_screenWidth * 0.95), _screenHeight * 0.97f, -1), glm::vec3(0, 0, 0), glm::vec3(_screenWidth * 0.1f, _screenHeight * 0.1f, 1), ResourceManager::GetGeometry("Quad"), std::make_shared<RenderDescription>(ShaderEnum::Default, Camera::Types::Orthographic));
	AddGameObject(_dificultyUI);
}

void GameScene::CollisionDetection()
{
	_QuadTree->HandleCollision();
	for (int i = 0; i < _collidableObjects.size(); ++i)
	{
		glm::vec3 collisionVector;
		// Check of collidable objects has moved outside of play area
		auto collidableI = std::static_pointer_cast<CollidableObject>(_collidableObjects.at(i));
		if (CollisionHandler::ScreenBoundCheck(collidableI, _screenBoundsX, _screenBoundsY, collisionVector) && collidableI->getScreenBound())
			collidableI->ScreenBound(collisionVector); // Try and bound object to screen, implemented in derived classes
	}
}

void GameScene::OnUpdateFrame(double deltaTime)
{
	_totalTime += deltaTime;
	// Check for collisions before updating GameObjects

	_dificultyUI->EditText(std::to_string(_difficultLevel)); //TODO::past actual level here

	CollisionDetection();

	// UpdateAI based on predefined behaviors
	// Must occur before Update loop to ensure updated transform values
	for (auto &i : _ai)
	{
		if (auto ai = std::static_pointer_cast<AI>(i))
			ai->UpdateAI(_players, deltaTime);
	}

	// Update GameObjects - Could be migrated to the baseScene.cpp
	for (int i = 0; i < _objects.size(); i++)
	{
		if (_objects.at(i)->alive())
			_objects.at(i)->Update(deltaTime);
	}

	for (auto &i : _collidableObjects)
	{
		if (i->moved() && i->alive())
		{
			RecalculateQuadTreePartitionLocation(i);
		}
	}

	//GameOver
	int countAlive = 0;
	int highScore = 0;
	int winningPlayer = 0;
	for(int i = 0; i < _players.size();i++)
	{
		countAlive += _players[i]->alive();
		if(_players[i]->GetScore() > highScore)
		{
			highScore = _players[i]->GetScore();
			winningPlayer = i + 1;
		}
	}
	if(countAlive == 0 && !_players.empty())
	{
		//GAME OVER!!!!!
		SceneManager::AddScene(std::make_shared<GameOverScene>(SceneManager::GetScreenWidth(), SceneManager::GetScreenHeight(), SceneManager::GetScreenBoundsX(), SceneManager::GetScreenBoundsY(), highScore, winningPlayer), "GameOverScene");
		SceneManager::ChangeScene("GameOverScene", true);
	}

	_difficultLevel = _totalTime / 10;

}

void GameScene::RecalculateQuadTreePartitionLocation(const std::shared_ptr<CollidableObject>& collidableObject) const
{
	collidableObject->RemoveFromCollisionCells();
	_QuadTree->Add(collidableObject);
}

void GameScene::OnRenderFrame(const std::shared_ptr<Renderer>& renderer, float dt)
{
	renderer->Render(_renderBatch,dt);
}

void GameScene::GetPowerUpTextureName(PowerUpType powerUpType, ShootMode shootMode, std::string& powerUpTexture)
{
	switch (powerUpType)
	{
	case PowerUpType::ShootMode:
		switch (shootMode)
		{
		case ShootMode::SingleShot:
			powerUpTexture = "SingleShot.png";
			break;
		case ShootMode::Burst:
			powerUpTexture = "BurstShot.png";
			break;
		case ShootMode::RapidFire:
			powerUpTexture = "RapidShot.png";
			break;
		case ShootMode::TriShot:
			powerUpTexture = "TriSingleShot.png";
			break;
		case ShootMode::TriShotBurst:
			powerUpTexture = "TriBurstShot.png";
			break;
		case ShootMode::TriShotRapidFire:
			powerUpTexture = "TriRapidShot.png";
			break;
		}
		break;
	case PowerUpType::Shield:
		powerUpTexture = "Shield.png";
		break;
	case PowerUpType::Speed:
		powerUpTexture = "SpeedBoost.png";
		break;
	case PowerUpType::GravityBomb:
		powerUpTexture = "GravBomb.png";
		break;
	}
}

void GameScene::SpawnPowerUp()
{
	auto spawnCell = GlobalStructures::GetSpawnCell();

	if (spawnCell)
	{
		spawnCell->blocked = true;
		auto powerUp = std::make_shared<PowerUp>(true, 0.1, spawnCell->position, glm::quat(0, 0, 0, 1.0), glm::vec3(0.1, 0.1, 0.1), glm::vec4(0.0, 0.0, 0.0, 1.0), AssetFactory::LoadGeometry_VTN("Assets/Models/SquareTorus.obj"), std::make_shared<RenderDescription>(ShaderEnum::Default));
		auto  powerUpType = static_cast<PowerUpType>(rand() % static_cast<int>(PowerUpType::Last));
		auto shootMode = static_cast<ShootMode>(rand() % static_cast<int>(ShootMode::Last));
		powerUp->SetPowerUpType(powerUpType);
		powerUp->SetShootMode(shootMode);
		std::string powerUpTexture = "";
		GetPowerUpTextureName(powerUpType, shootMode, powerUpTexture);
		powerUp->setTextureName(powerUpTexture);
		AddGameObject(powerUp);
		return;
	}
}

void GameScene::SpawnAI()
{
	auto spawnCell = GlobalStructures::GetSpawnCell(true);
	std::shared_ptr<Geometry> geometry;
	if (spawnCell)
	{
		//spawnCell->blocked = true;
		srand(time(NULL));
		const auto randAI = rand() % ((4 - 1) + 1) + 1;
		AIBehaviours behaviour = Shrapnel;
		switch (randAI)
		{
		case 1:
			behaviour = Shrapnel;
			geometry = 	AssetFactory::LoadGeometry_VTN("Assets/Models/BensExplosion.obj");
			break;
		case 2:
			behaviour = Rook;
			geometry = 	AssetFactory::LoadGeometry_VTN("Assets/Models/BensBearTrap.obj");
			break;
		case 3:
			behaviour = Shuriken;
			geometry = 	AssetFactory::LoadGeometry_VTN("Assets/Models/DodecahedronTwo.obj");
			break;
		case 4:
			behaviour = Rupture;
			geometry = 	AssetFactory::LoadGeometry_VTN("Assets/Models/BensDankLeaf.obj");
			break;
		default:
			behaviour = Shrapnel;
			geometry = 	AssetFactory::LoadGeometry_VTN("Assets/Models/DodecahedronFour.obj");
			break;
		}

		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);


		for (auto &i : _ai)
		{
			if (!i->alive())
			{
				i->alive(true);
				i->setPosition(spawnCell->position);
				i->setBehaviour(behaviour);
				i->setGeometry(geometry);
				i->setColour(glm::vec4(r, g, b, 1.0));
				i->Reset();
				return;
			}
		}
		AddGameObject(std::make_shared<AI>(true, 0.1, spawnCell->position, glm::vec3(3.1415f / 2, 0.0f, 0.0f),
			glm::vec3(0.1, 0.1, 0.1), glm::vec4(r, g, b, 1.0),
			geometry,
			std::make_shared<RenderDescription>(ShaderEnum::Default), behaviour));
		return;
	}
}

void GameScene::SpawnAIAtPosition(const glm::vec3& position, const AIBehaviours behaviour /*= Undefined*/)
{
	auto spawnCell = GlobalStructures::GetSpawnCellNearLocation(position, true);
	std::shared_ptr<Geometry> geometry;
	if (spawnCell)
	{
		AIBehaviours aiBehaviour = behaviour;
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		if (behaviour == Undefined)
		{
			const auto randAI = rand() % ((3 - 1) + 1) + 1;
			switch (randAI)
			{
			case 1:
				aiBehaviour = Shrapnel;
				geometry = 	AssetFactory::LoadGeometry_VTN("Assets/Models/DodecahedronFour.obj");
				break;
			case 2:
				aiBehaviour = Rook;
				geometry = 	AssetFactory::LoadGeometry_VTN("Assets/Models/DodecahedronEight.obj");
				break;
			case 3:
				aiBehaviour = Shuriken;
				geometry = 	AssetFactory::LoadGeometry_VTN("Assets/Models/DodecahedronTwelve.obj");
				break;
			default:
				aiBehaviour = Shrapnel;
				geometry = 	AssetFactory::LoadGeometry_VTN("Assets/Models/DodecahedronFour.obj");
				break;
			}
		}

		for (auto &i : _ai)
		{
			if (!i->alive())
			{
				i->alive(true);
				i->setPosition(spawnCell->position);
				i->setBehaviour(behaviour);
				i->setGeometry(geometry);
				i->setColour(glm::vec4(r, g, b, 1.0));
				return;
			}
		}
		AddGameObject(std::make_shared<AI>(true, 0.1, spawnCell->position, glm::quat(0, 0, 0, 1.0),
			glm::vec3(0.1, 0.1, 0.1), glm::vec4(r, g, b, 1.0),
			geometry,
			std::make_shared<RenderDescription>(ShaderEnum::Default), aiBehaviour));
	}
}

void GameScene::SpawnGravityBomb(const glm::vec3& position)
{
	auto spawnCell = GlobalStructures::GetSpawnCellNearLocation(position, true);

	if (spawnCell)
	{
		spawnCell->blocked = true;

		AddGameObject(std::make_shared<GravityBomb>(true, 0.4, spawnCell->position, glm::quat(0, 0, 0, 1.0), glm::vec3(0.1, 0.1, 0.1), glm::vec4(0.431f, 0.3f, 0.76f, 0.8), ResourceManager::GetGeometry("DodecahedronFour.obj"), std::make_shared<RenderDescription>(ShaderEnum::Default)));
		return;
	}
}

void GameScene::AddBullet(const glm::vec3 position, const glm::vec2 direction, const int playerID, const glm::vec4& playerColour)
{
	for (auto &i : _bullets)
	{
		if (!i->alive())
		{
			i->alive(true);
			i->setPosition(position);
			i->setVelocity(glm::vec3(direction.x, direction.y, 0));
			i->setPlayerID(playerID);
			i->StorePlayerColour(playerColour);
			AudioManager::PlaySound("Assets/Audio/LaserShoot.wav");
			return;
		}
	}
	AddGameObject(std::make_shared<Bullet>(playerID, direction, true, 0.025, position, glm::quat(0, 0, 0, 1.0), glm::vec3(0.05, 0.05, 0.05), glm::vec4(0.1, 0.1, 0.1, 1.0), AssetFactory::LoadGeometry_VTN("Assets/Models/Tetrahedron.obj"), std::make_shared<RenderDescription>(ShaderEnum::Default)));
	AudioManager::PlaySound("Assets/Audio/LaserShoot.wav");

}

void GameScene::AddGameObject(const std::shared_ptr<Object>& gameObject)
{
	BaseScene::AddGameObject(gameObject);
	if (auto collidableObject = std::dynamic_pointer_cast<CollidableObject>(gameObject))
	{
		_collidableObjects.push_back(collidableObject); // Add collidable objects to separate collection
		_QuadTree->Add(collidableObject);
	}

	if (auto player = std::dynamic_pointer_cast<Player>(gameObject))
		_players.push_back(player); // Add player to separate collection

	if (auto ai = std::dynamic_pointer_cast<AI>(gameObject))
		_ai.push_back(ai); // Add ai to separate collection

	if (const auto playerZone = std::dynamic_pointer_cast<PlayerZone>(gameObject))
	{
		m_playerZones.push_back(playerZone);
	}

	if (const auto particleSystem = std::dynamic_pointer_cast<ParticleSystem>(gameObject))
	{
		_particleSystem.push_back(particleSystem); //Add Particlesystem to separate collection
	}

	if (const auto bullet = std::dynamic_pointer_cast<Bullet>(gameObject))
	{
		_bullets.push_back(bullet);
	}
}

void GameScene::RemoveGameObject(const std::shared_ptr<Object>& gameObject)
{

	BaseScene::RemoveGameObject(gameObject);

	for (auto i = 0; i < _collidableObjects.size(); i++)
	{
		if (_collidableObjects.at(i) == std::dynamic_pointer_cast<CollidableObject>(gameObject))
		{
			_collidableObjects.erase(_collidableObjects.begin() + i);
		}
	}

	for (auto i = 0; i < _players.size(); i++)
	{
		if (_players.at(i) == std::dynamic_pointer_cast<Player>(gameObject))
		{
			_players.erase(_players.begin() + i);
		}
	}

	for (auto i = 0; i < m_playerZones.size(); i++)
	{
		if (m_playerZones.at(i) == std::dynamic_pointer_cast<PlayerZone>(gameObject))
		{
			m_playerZones.erase(m_playerZones.begin() + i);
		}
	}

	for (auto i = 0; i < m_powerUps.size(); i++)
	{
		if (m_powerUps.at(i) == std::dynamic_pointer_cast<PowerUp>(gameObject))
		{
			m_powerUps.erase(m_powerUps.begin() + i);
		}
	}

	for (auto i = 0; i < _ai.size(); i++)
	{
		if (_ai.at(i) == gameObject)
		{
			_ai.erase(_ai.begin() + i);
		}
	}

	for (auto i = 0; i < _particleSystem.size(); i++)
	{
		if (_particleSystem.at(i) == std::dynamic_pointer_cast<ParticleSystem>(gameObject))
		{
			_particleSystem.erase(_particleSystem.begin() + i);
		}
	}

	for (auto i = 0; i < _bullets.size(); i++)
	{
		if (_bullets.at(i) == std::dynamic_pointer_cast<Bullet>(gameObject))
		{
			_bullets.erase(_bullets.begin() + i);
		}
	}
}

std::shared_ptr<Player> GameScene::GetPlayerByID(const int playerID)
{
	for (auto &i : _players)
	{
		if (i->getPlayerID() == playerID)
			return i;
	}

	return nullptr;

}

void GameScene::Reset()
{
}

void GameScene::Shutdown()
{
	RemoveGameObject(m_particleSystemManager);
}

void GameScene::OnSwitchIn()
{
	InputManager::InitializeDirectInputGamePads();//Lambda functions for game mode 2 (hot swapping)
	m_createPlayerFunction = [this](const int playerID)
	{
		auto oldPlayerID = -1;
		auto foundPlayerObject = false;

		//First check if we have an available player object we can reset/ takeover
		for (auto& playerObject : _players)
		{
			const auto& player = std::dynamic_pointer_cast<Player>(playerObject);

			if (!player->GetPlayerConnected())
			{
				//Save old playerID for playerzone
				oldPlayerID = player->getPlayerID();

				const auto spawnCell = GlobalStructures::GetSpawnCell(false);

				if (spawnCell)
				{
					spawnCell->blocked = true;

					//Activate player
					player->setPlayerID(playerID);
					player->SetPlayerConnected(true);
					player->alive(true);
					player->setHealth(100.0f);
					player->setScore(0);
					player->setPosition(spawnCell->position);
					player->setRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
					//We can reset and take over

					player->SetSpawnCell(spawnCell);

					//Create playerzone
					const auto newPlayerZone = std::make_shared<PlayerZone>(0.3f, spawnCell->position, glm::vec3(0.6f, 0.6f, 0.6f), player->getColour(),
						ResourceManager::GetGeometry("CirclePlane.obj"), std::make_shared<RenderDescription>(ShaderEnum::Default));

					newPlayerZone->SetOwnedPlayerID(playerID);
					newPlayerZone->alive(true);
					newPlayerZone->SetSpawnCell(spawnCell);

					AddGameObject(newPlayerZone);

					player->SetPlayerZone(newPlayerZone);

					foundPlayerObject = true;
				}

				return;
			}
		}

		////Find playerZone
		//if (foundPlayerObject)
		//{
		//	for (auto& playerZone : m_playerZones)
		//	{
		//		if (playerZone->GetOwnedPlayerID() == oldPlayerID)
		//		{
		//			//Activate playerZone
		//			playerZone->SetOwnedPlayerID(playerID);
		//			playerZone->alive(true);
		//			playerZone->setPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
		//			playerZone->setRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

		//			return;
		//		}
		//	}
		//}

		//Else we create a new player

		const auto spawnCell = GlobalStructures::GetSpawnCell(false);

		if (spawnCell)
		{
			glm::vec4 playerColour = glm::vec4(std::rand() % 100 / 100.0, std::rand() % 100 / 100.0, std::rand() % 100 / 100.0, 1);// added for ui but would be good to sync this up
			spawnCell->blocked = true;
			auto uiHealth = std::make_shared<int>(10);
			auto uiScore = std::make_shared<int>(0);
			auto uiPowerUpPictures = std::make_shared<std::vector<std::wstring>>();
			//TODO:: replace with good empty slot texture
			(*uiPowerUpPictures).emplace_back(L"Empty.png");
			(*uiPowerUpPictures).emplace_back(L"Empty.png");
			(*uiPowerUpPictures).emplace_back(L"Empty.png");
			(*uiPowerUpPictures).emplace_back(L"Empty.png");
			const auto newPlayer = std::make_shared<Player>(uiHealth, uiScore, uiPowerUpPictures, true, 0.1, 0.05f, spawnCell->position, glm::vec3(3.1415f / 2, 0.0f, 0.0f), glm::vec3(0.1), playerColour,
				ResourceManager::GetGeometry("TorusKnot.obj"), std::make_shared<RenderDescription>(ShaderEnum::Default));

			newPlayer->SetPlayerConnected(true);
			newPlayer->setPlayerID(playerID);
			newPlayer->alive(true);
			newPlayer->SetSpawnCell(spawnCell);

			AddGameObject(newPlayer);

			//Create playerzone
			const auto newPlayerZone = std::make_shared<PlayerZone>(0.3f, spawnCell->position, glm::vec3(0.6f, 0.6f, 0.6f), playerColour,
				ResourceManager::GetGeometry("CirclePlane.obj"), std::make_shared<RenderDescription>(ShaderEnum::Default));

			newPlayerZone->SetOwnedPlayerID(playerID);
			newPlayerZone->alive(true);
			newPlayerZone->SetSpawnCell(spawnCell);

			AddGameObject(newPlayerZone);

			newPlayer->SetPlayerZone(newPlayerZone);

			auto found = _uiHealth.find(playerID);
			if (found == _uiHealth.end())
			{
				//Health bar
				const auto livesBar = std::make_shared<LivesBar>(uiHealth, uiScore, uiPowerUpPictures, glm::vec3(0, (_screenHeight / 8) * (playerID), -1), *uiHealth, 0.5, playerColour, "Player" + std::to_string(playerID + 1));
				_uiHealth[playerID] = livesBar;
				for (const auto& go : livesBar->GetSubjects())
				{
					AddGameObject(go);
				}
				AddGameObject(livesBar);
			}
			else
			{
				found->second->ChangeHealthPointer(uiHealth);
				found->second->ChangeScorePointer(uiScore);
				found->second->ChangeTextureNames(uiPowerUpPictures);
			}
		}
	};

	m_destroyPlayerFunction = [this](const int playerID)
	{
		int aliveCount = 0;
		//Remove player
		for (auto i = 0; i < _players.size(); i++)
		{
			const auto& player = std::dynamic_pointer_cast<Player>(_players.at(i));

			if (player->getPlayerID() == playerID)
			{
				player->SetPlayerConnected(false);
				player->setPlayerID(-1);

				if (player->alive())
				{
					player->alive(false);
					player->DropPowerUp();

					player->GetPlayerZone()->DropAllPowerUps();

					player->GetPlayerZone()->RemoveFromCollisionCells();
				}

				player->GetSpawnCell()->blocked = false;
				player->SetSpawnCell(nullptr);

				RemoveGameObject(player->GetPlayerZone());

				player->SetPlayerZone(nullptr);

				return;
			}
		}

		////Remove playerzone
		//for (auto i = 0; i < m_playerZones.size(); i++)
		//{
		//	const auto& playerZone = m_playerZones.at(i);

		//	if (playerZone->GetOwnedPlayerID() == playerID)
		//	{
		//		playerZone->SetOwnedPlayerID(-1);
		//		playerZone->alive(false);
		//		return;
		//	}
		//}
	};

	InputManager::RegisterCreatePlayerFunction(m_createPlayerFunction);
	InputManager::RegisterDestroyPlayerFunction(m_destroyPlayerFunction);
}

void GameScene::OnSwitchOut()
{
}
