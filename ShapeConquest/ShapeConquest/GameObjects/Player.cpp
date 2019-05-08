#include "Player.h"
#include "../Bullet.h"
#include "../Utilities/AssetFactory.h"
#include "../GameWindow.h"
#include "../GameScene.h"
#include "PowerUp.h"
#include "PlayerZone.h"
#include "../AudioManager.h"

Player::Player(std::shared_ptr<int> uiHealth, std::shared_ptr<int> score, std::shared_ptr<std::vector<wstring>> textureNames, bool screenBound, float radius, const glm::vec3 position, const glm::quat rotation, const glm::vec3 scale, glm::vec4 color,
	std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc)
	: MoveableObject(screenBound, radius, position, rotation, scale, color, geometry, rendDesc), _PlayerID(0), m_playerInSafeZone(false), m_playerHoldingPowerUp(false), _PlayerName("Default"), _Forward(0, 0, 0), m_powerUp(nullptr), _Radius(radius),
	_uiHealth(std::move(uiHealth)), _uiScore(std::move(score)), _uiPickUpTextureNames(std::move(textureNames))
{
	_entityType = static_cast<short>(EntityCategory::Player);
	_collisionMask = static_cast<short>(EntityCategory::AI) | static_cast<glm::uint16>(EntityCategory::PowerUp) | static_cast<glm::uint16>(EntityCategory::PlayerZone);

	auto result = _entityType & _collisionMask;

}

Player::Player(std::shared_ptr<int> uiHealth, std::shared_ptr<int> score, std::shared_ptr<std::vector<wstring>> textureNames, bool screenBound, float radius, float movementSpeed, const glm::vec3 position, const glm::quat rotation, const glm::vec3 scale, glm::vec4 color,
	std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc)
	: MoveableObject(screenBound, radius, movementSpeed, position, rotation, scale, color, geometry, rendDesc), _PlayerID(0), m_playerInSafeZone(false), m_playerHoldingPowerUp(false), _PlayerName("Default"), _Forward(0, 0, 0), m_powerUp(nullptr), _Radius(radius),
	_uiHealth(std::move(uiHealth)), _uiScore(std::move(score)), _uiPickUpTextureNames(std::move(textureNames))
{
	_entityType = static_cast<short>(EntityCategory::Player);
	_collisionMask = static_cast<short>(EntityCategory::AI) | static_cast<glm::uint16>(EntityCategory::PowerUp) | static_cast<glm::uint16>(EntityCategory::PlayerZone);
	auto result = _entityType & _collisionMask;
	_defaultMovementSpeed = movementSpeed;
}

Player::~Player()
{
	GameObject::~GameObject();
}

void Player::RestetPickupUi(const std::wstring& texName) const
{
	for (auto& ui : *_uiPickUpTextureNames)
	{
		ui = texName;
	}
}

void Player::Update(const double dt)
{
	MoveableObject::Update(dt);

	*_uiHealth = static_cast<int>(_Health / _maxHealth * 10);
	*_uiScore = static_cast<int>(_Score);

	m_timeSincePowerUp += dt;
	m_timeSinceDropped += dt;
	m_timeSincePickup += dt;

	//Check for playerzone collision
	if (m_playerZone)
	{
		const auto distance = _Position - m_playerZone->getPosition();

		const auto size = glm::length(distance);

		const auto radiusSum = _Radius + m_playerZone->getRadius();

		if (size * size <= radiusSum * radiusSum)
		{
			m_playerInSafeZone = true;

			m_playerZone->setColour(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

			m_timeInSafeZone += dt;

			if (m_timeInSafeZone > m_playerZoneTimeLimit)
			{
				m_playerZone->setColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

				_Health -= 4.0f * dt;
			}
		}
		else
		{
			m_playerInSafeZone = false;
			m_playerZone->ChangeToColourDefault();
			m_timeInSafeZone = 0.0f;
		}
	}

	// Add logic here for general updating

	if (_Health <= 0)
	{
		alive(false);

		if (m_playerHoldingPowerUp)
		{

			DropPowerUp();
		}

		m_playerZone->DropAllPowerUps();

		m_playerZone->RemoveFromCollisionCells();

		if (const auto standardScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
		{
			standardScene->RemoveGameObject(m_playerZone);
		}

		m_playerZone = nullptr;

		return;
	}

	HandleInput(dt);
	_velocity += _Forward * getMovementSpeed();
	_Position += _velocity * static_cast<float>(dt);

	if (m_playerHoldingPowerUp)
	{
		const auto invertDirection = _oldPosition - _Position;
		const auto dirNorm = glm::normalize(invertDirection);
		m_powerUp->setPosition(_Position + (dirNorm * _Radius * 3.0f));
		if (InputManager::QueryButton(_PlayerID, Button::B) == ButtonState::Pressed && m_timeSincePickup > 1.0f)
		{
			DropPowerUp();
			m_timeSinceDropped = 0.0f;
			//(*_uiPickUpTextureNames)[number] = L"TextName"; //TODO::implement pickup textures
		}
	}

	if (glm::length(_velocity) > 0.001f)
		moved(true);
	if (!_collided)
		_oldPosition = _Position;


	_velocity *= 0.99f;

	_timeSinceLastShot += dt;


	if (_shootMode != _defaultMode && _timeSinceShootModeChanged > _shootPowerUpTime)
		_shootMode = _defaultMode;

	_timeSinceShootModeChanged += dt;

	if (_MovementSpeed != _defaultMovementSpeed && _timeSinceSpeedBoost > _speedBoostTimeLimit)
		_MovementSpeed = _defaultMovementSpeed;

	_timeSinceSpeedBoost += dt;

	_collided = false;
}

void Player::HandleInput(const double dt)
{
	if ((InputManager::QueryRightStickPosition(_PlayerID)) != glm::vec2(0, 0))
	{
		glm::vec2 direction = InputManager::QueryRightStickPosition(_PlayerID);
		direction = glm::normalize(direction);
		switch (_shootMode)
		{
		case ShootMode::RapidFire:
			if (const auto standardScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
				standardScene->AddBullet(_Position + glm::vec3((glm::normalize(direction) * getRadius()), 0.0), direction, _PlayerID, _Colour);
			break;
		case ShootMode::Burst:

			if (_bulletsShot != 5 || _timeSinceLastShot > 0.2f)
			{
				if (_bulletsShot == 5 && _timeSinceLastShot > 0.5f)
					_bulletsShot = 0;
				else
					_bulletsShot++;

				_timeSinceLastShot = 0;
				if (const auto standardScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
					standardScene->AddBullet(_Position + glm::vec3((glm::normalize(direction) * getRadius()), 0.0), direction, _PlayerID, _Colour);
			}
			break;
		case ShootMode::SingleShot:
			if (_timeSinceLastShot > 0.2f)
			{
				_timeSinceLastShot = 0.0f;
				if (const auto standardScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
					standardScene->AddBullet(_Position + glm::vec3((glm::normalize(direction) * getRadius()), 0.0), direction, _PlayerID, _Colour);
			}
			break;
		case ShootMode::TriShot:
			if (_timeSinceLastShot > 0.2f)
			{
				_timeSinceLastShot = 0.0f;
				auto direction2 = direction + (glm::vec2(direction.y, direction.x) * 0.2f);
				//direction2 = glm::normalize(direction2);
				auto direction1 = direction - (glm::vec2(direction.y, direction.x) * 0.2f);
				//direction1 = glm::normalize(direction1);
				if (const auto standardScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
					standardScene->AddBullet(_Position + glm::vec3((glm::normalize(direction) * getRadius()), 0.0), direction, _PlayerID, _Colour);
				if (const auto standardScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
					standardScene->AddBullet(_Position + glm::vec3((glm::normalize(direction2) * getRadius()), 0.0), direction2, _PlayerID, _Colour);
				if (const auto standardScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
					standardScene->AddBullet(_Position + glm::vec3((glm::normalize(direction1) * getRadius()), 0.0), direction1, _PlayerID, _Colour);
			}
			break;
		case ShootMode::TriShotBurst:
			if (_bulletsShot != 5 || _timeSinceLastShot > 0.5f)
			{
				if (_bulletsShot == 5 && _timeSinceLastShot > 0.5f)
					_bulletsShot = 0;
				else
					_bulletsShot++;

				_timeSinceLastShot = 0;

				auto direction2 = direction + (glm::vec2(direction.y, direction.x) * 0.2f);
				//direction2 = glm::normalize(direction2);
				auto direction1 = direction - (glm::vec2(direction.y, direction.x) * 0.2f);
				//direction1 = glm::normalize(direction1);
				if (const auto standardScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
					standardScene->AddBullet(_Position + glm::vec3((glm::normalize(direction) * getRadius()), 0.0), direction, _PlayerID, _Colour);
				if (const auto standardScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
					standardScene->AddBullet(_Position + glm::vec3((glm::normalize(direction2) * getRadius()), 0.0), direction2, _PlayerID, _Colour);
				if (const auto standardScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
					standardScene->AddBullet(_Position + glm::vec3((glm::normalize(direction1) * getRadius()), 0.0), direction1, _PlayerID, _Colour);
			}
			break;
		case ShootMode::TriShotRapidFire:

			auto direction2 = direction + (glm::vec2(direction.y, direction.x) * 0.2f);
			//direction2 = glm::normalize(direction2);
			auto direction1 = direction - (glm::vec2(direction.y, direction.x) * 0.2f);
			//direction1 = glm::normalize(direction1);
			if (const auto standardScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
				standardScene->AddBullet(_Position + glm::vec3((glm::normalize(direction) * getRadius()), 0.0), direction, _PlayerID, _Colour);
			if (const auto standardScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
				standardScene->AddBullet(_Position + glm::vec3((glm::normalize(direction2) * getRadius()), 0.0), direction2, _PlayerID, _Colour);
			if (const auto standardScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
				standardScene->AddBullet(_Position + glm::vec3((glm::normalize(direction1) * getRadius()), 0.0), direction1, _PlayerID, _Colour);
			break;
		}
		//auto bullet = std::make_shared<Bullet>(direction, true, 0.1, this->getPosition(), glm::quat(0, 0, 0, 1.0), glm::vec3(0.1, 0.1, 0.1), AssetFactory::LoadGeometry_VTN("Assets/Models/DodecahedronFour.obj"), std::make_shared<RenderDescription>(ShaderEnum::TestPink));

	}

	// Left-Thumbstick = movement
	auto xAxis = InputManager::QueryLeftStickX(_PlayerID);
	xAxis = (glm::abs(xAxis) >= 0.1f) ? xAxis : 0;
	auto yAxis = InputManager::QueryLeftStickY(_PlayerID);
	yAxis = (glm::abs(yAxis) >= 0.1f) ? yAxis : 0;

	_Forward = glm::vec3(xAxis, yAxis, 0);

	if (InputManager::QueryButton(_PlayerID, Button::DPadRight) == ButtonState::Pressed && m_timeSincePowerUp > m_powerUpUseTime)
	{
		if (m_playerZone->UsePowerUp(PowerUpType::GravityBomb))
		{
			AudioManager::PlaySound("Assets/Audio/PowerupCollect.wav");
			if (const auto standardGameScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
			{
				standardGameScene->SpawnGravityBomb(_Position);
			}

			UpdateUIElement(PowerUpType::GravityBomb);
		}
	}

	if (InputManager::QueryButton(_PlayerID, Button::DPadDown) == ButtonState::Pressed && m_timeSincePowerUp > m_powerUpUseTime)
	{

		if (m_playerZone->UsePowerUp(PowerUpType::Shield))
		{
			AudioManager::PlaySound("Assets/Audio/PowerupCollect.wav");
			_shielded = true;
			UpdateUIElement(PowerUpType::Shield);
		}
	}

	if (InputManager::QueryButton(_PlayerID, Button::DPadLeft) == ButtonState::Pressed && m_timeSincePowerUp > m_powerUpUseTime)
	{

		std::shared_ptr<PowerUp> powerUp;
		if (m_playerZone->UseAndGetPowerUp(PowerUpType::ShootMode, powerUp))
		{
			if (powerUp)
			{
				AudioManager::PlaySound("Assets/Audio/PowerupCollect.wav");
				_shootMode = powerUp->GetShootMode();
				_timeSinceShootModeChanged = 0.0f;
				UpdateUIElement(PowerUpType::ShootMode);
			}
		}
	}

	if (InputManager::QueryButton(_PlayerID, Button::DPadUp) == ButtonState::Pressed && m_timeSincePowerUp > m_powerUpUseTime)
	{
		AudioManager::PlaySound("Assets/Audio/PowerupCollect.wav");
		if (m_playerZone->UsePowerUp(PowerUpType::Speed))
		{
			_MovementSpeed = _MovementSpeed * 2;
			_timeSinceSpeedBoost = 0.0f;

			UpdateUIElement(PowerUpType::Speed);
		}
	}

	/*if (InputManager::QueryButton(_PlayerID, Button::A) == ButtonState::Pressed)
	{
		SceneManager::TogglePause();
	}*/
}

void Player::OnCollide(const CollisionData& collisionData)
{
	if (alive())
	{
		const auto& collidedObject = collisionData.CollidedObjects[1];

		if (m_playerHoldingPowerUp)
		{
			if (collidedObject->getEntityType() == static_cast<glm::uint16>(EntityCategory::PlayerZone))
			{
				if (m_playerZone->GetOwnedPlayerID() == _PlayerID)
				{
					AudioManager::PlaySound("Assets/Audio/DepositPowerUp.wav");
					m_playerInSafeZone = true;
					auto powerUpName = m_powerUp->getTextureName();
					std::wstring wide_string = std::wstring(powerUpName.begin(), powerUpName.end());
					int index = static_cast<int>(m_powerUp->GetPowerUpType());
					(*_uiPickUpTextureNames)[index] = wide_string;
					//Send powerup to playerZone
					m_playerZone->AddPowerUp(m_powerUp);
					m_powerUp->alive(false);
					m_powerUp = nullptr;

					m_timeSinceDropped = 0.0f;
					m_playerHoldingPowerUp = false;
				}
			}
		}
		else
		{
			if (collidedObject->getEntityType() == static_cast<short>(EntityCategory::PowerUp))
			{
				if (m_timeSinceDropped > 1.0f)
				{
					AudioManager::PlaySound("Assets/Audio/PowerupPickup.wav");
					m_playerHoldingPowerUp = true;
					m_powerUp = std::dynamic_pointer_cast<PowerUp>(collisionData.CollidedObjects[1]);
					m_timeSincePickup = 0.0f;
				}
				else
					std::cout << "Not enough time past" << std::endl;
				/*if (InputManager::QueryButton(_PlayerID, Button::A) == ButtonState::Pressed)
				{

				}*/
			}
		}
	}

	// Handle collision response here
	//std::cout << "Collided" << std::endl;
	//_Position -= _velocity;
}

void Player::ScreenBound(const glm::vec3& collisionVector)
{
	// Add functionality to prevent player leaving the screen-play area
	MoveableObject::ScreenBound(collisionVector);
	_collided = true;
}

const std::shared_ptr<PlayerZone>& Player::GetPlayerZone() const
{
	return m_playerZone;
}

void Player::SetPlayerZone(const std::shared_ptr<PlayerZone>& playerZone)
{
	m_playerZone = playerZone;
}

bool Player::SetPowerUp(const std::shared_ptr<PowerUp>& powerUp)
{
	if (m_powerUp)
	{
		m_powerUp = powerUp;
		return true;
	}

	return false;
}

const std::shared_ptr<PowerUp>& Player::GetPowerUp() const
{
	return m_powerUp;
}

void Player::DropPowerUp()
{
	if (m_powerUp)
	{
		const auto spawnCell = GlobalStructures::GetSpawnCellNearLocation(m_powerUp->getPosition());

		if (spawnCell)
		{
			spawnCell->blocked = true;

			m_powerUp->setPosition(spawnCell->position);

			const auto scene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene());
			scene->RecalculateQuadTreePartitionLocation(m_powerUp);

			m_powerUp = nullptr;
			m_playerHoldingPowerUp = false;
		}
	}
}

void Player::UpdateUIElement(const PowerUpType& powerUp)
{
	std::string texture = "";
	if (m_playerZone->HasPowerUp(powerUp, texture))
	{
		std::wstring wide_string = std::wstring(texture.begin(), texture.end());
		int index = static_cast<int>(powerUp);
		(*_uiPickUpTextureNames)[index] = wide_string;
	}
	else
	{
		texture = "Empty.png";
		std::wstring wide_string = std::wstring(texture.begin(), texture.end());
		int index = static_cast<int>(powerUp);
		(*_uiPickUpTextureNames)[index] = wide_string;
	}
}
