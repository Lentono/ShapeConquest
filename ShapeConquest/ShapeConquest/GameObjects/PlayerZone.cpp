#include "PlayerZone.h"
#include "../Managers/SceneManager.h"
#include "../Scenes/StandardGameScene.h"


PlayerZone::PlayerZone(const float radius, const glm::vec3 position, const glm::vec3 scale, const glm::vec4 colour, const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<RenderDescription>& rendDesc)
	: CollidableObject(true, radius, position, glm::angleAxis(glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)), scale, colour, geometry, rendDesc), m_ownedPlayerID(-1), m_colourDefault(colour)
{
	_entityType = static_cast<glm::uint16>(EntityCategory::PlayerZone);
	_collisionMask = static_cast<short>(EntityCategory::Player);
}

PlayerZone::~PlayerZone()
{
}

int PlayerZone::GetOwnedPlayerID() const 
{
	return m_ownedPlayerID;
}

void PlayerZone::SetOwnedPlayerID(const int playerID) 
{
	m_ownedPlayerID = playerID;
}

void PlayerZone::AddPowerUp(const std::shared_ptr<PowerUp>& powerUp)
{
	m_powerUps.emplace_back(powerUp);
	std::cout << "Collected Power Up of Type: ";

	switch(powerUp->GetPowerUpType())
	{
	case PowerUpType::ShootMode:
			std::cout << "ShootMode" << std::endl;
			break;
	case PowerUpType::Shield:
		std::cout << "Shield" << std::endl;
		break;
	case PowerUpType::Speed:
		std::cout << "Speed" << std::endl;
		break;
	case PowerUpType::GravityBomb:
		std::cout << "GravityBomb" << std::endl;
		break;
	}
	auto test = false;
}

const std::shared_ptr<PowerUp>& PlayerZone::GetPowerUpAt(const unsigned at) const
{
	return m_powerUps.at(at);
}

const std::vector<shared_ptr<PowerUp>>& PlayerZone::GetAllPowerUps() const
{
	return m_powerUps;
}

bool PlayerZone::UseAndGetPowerUp(const PowerUpType& powerUpType, std::shared_ptr<PowerUp>& powerUp)
{
	for (unsigned int i = 0; i < m_powerUps.size(); i++)
	{
		auto& currentPower = m_powerUps.at(i);

		if (currentPower)
		{
			if (currentPower->GetPowerUpType() == powerUpType)
			{
				powerUp = currentPower;
				m_powerUps.erase(m_powerUps.begin() + i);
				currentPower = nullptr;
				return true;
			}
		}
		else
			m_powerUps.erase(m_powerUps.begin() + i);
	}

	return false;
}

bool PlayerZone::UsePowerUp(const PowerUpType& powerUpType) 
{
	for (unsigned int i = 0; i < m_powerUps.size(); i++)
	{
		auto& powerUp = m_powerUps.at(i);

		if (powerUp)
		{
			if (powerUp->GetPowerUpType() == powerUpType)
			{
				powerUp = nullptr;
				m_powerUps.erase(m_powerUps.begin() + i);
				return true;
			}
		}
		else
			m_powerUps.erase(m_powerUps.begin() + i);

	}

	return false;
}


bool PlayerZone::HasPowerUp(const PowerUpType& powerUpType, std::string& texture)
{
	for (unsigned int i = 0; i < m_powerUps.size(); i++)
	{
		auto& powerUp = m_powerUps.at(i);

		if (powerUp)
		{
			if (powerUp->GetPowerUpType() == powerUpType)
			{
				texture = powerUp->getTextureName();
				return true;
			}
		}	
	}
	return false;
}

void PlayerZone::DropAllPowerUps()
{
	for (auto& powerUp : m_powerUps)
	{
		if (powerUp)
		{
			const auto spawnCell = GlobalStructures::GetSpawnCellNearLocation(_Position);

			if (spawnCell)
			{
				spawnCell->blocked = true;

				powerUp->setPosition(spawnCell->position);

				const auto scene = std::dynamic_pointer_cast<StandardGameScene>(SceneManager::ActiveScene());
				scene->RecalculateQuadTreePartitionLocation(powerUp);
				powerUp->alive(true);

				powerUp = nullptr;
			}
		}
	}

	m_powerUps.clear();
}

void PlayerZone::ChangeToColourDefault() 
{
	setColour(m_colourDefault);
}


void PlayerZone::OnCollide(const CollisionData& collisionData)
{
	
}

void PlayerZone::Update(const double dt)
{
	
}


