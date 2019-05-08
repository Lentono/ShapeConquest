#include "AI.h"
#include "../Bullet.h"
#include "../Managers/SceneManager.h"
#include "../Scenes/StandardGameScene.h"
#include "../AudioManager.h"


AI::AI(bool screenBound, float radius, glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec4 color,
	std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc, AIBehaviours aiBehaviour)
	: MoveableObject(screenBound, radius, position, rotation, scale, color, geometry, rendDesc), _aiBehaviour(aiBehaviour)
{

	_MovementSpeed = 1.0f;
	_entityType = static_cast<short>(EntityCategory::AI);
	_collisionMask = static_cast<short>(EntityCategory::Player) | static_cast<short>(EntityCategory::Bullet) | static_cast<short>(EntityCategory::Bomb);
	auto result = _entityType & _collisionMask;
}

AI::~AI()
{
}

void AI::Update(const double dt)
{
	MoveableObject::Update(dt);
	glm::vec3 scaledVelocity = glm::vec3(0.0);
	_oldPosition = _Position;
	//this->setPosition(this->getPosition() + (_velocity	*	static_cast<float>(dt)));
	_Position = this->getPosition() + (_velocity	*	static_cast<float>(dt));
	_collided = false;
	if (_oldPosition != _Position)
		moved(true);
	if (_Health <= 0)
	{
		alive(false);

	}
}

void AI::UpdateAI(const std::vector<std::shared_ptr<Player>>& players, const double dt)
{
	// AI behaviors defined here
	_time += dt;
	if (!_collided && !_trapped)
	{
		switch (_aiBehaviour)
		{
		case Rook:
			MoveTowardsClosestPlayer(players);
			if (!_closestPlayer)
				if (_velocity == glm::vec3(0.0))
				{
					float max = 2;
					float randX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max)) - 1.0f;
					float randY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max)) - 1.0f;
					_velocity += glm::vec3(randX, randY, 0);
				} 
			break;
		case Shuriken:
			MoveTowardsClosestPlayer(players);
			if (_closestPlayer)
			{
				const auto deltaRot = glm::angleAxis(static_cast<float>(_MovementSpeed * dt), glm::vec3(0, 0, 1));
				_Rotation *= deltaRot;

				_velocity = _velocity + (glm::sin(_time*10.0f)* (glm::vec3(_velocity.y, _velocity.x, 0) * 0.5f));
			}
			if (!_closestPlayer)
				if (_velocity == glm::vec3(0.0))
				{
					float max = 2;
					float randX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max)) - 1.0f;
					float randY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max)) - 1.0f;
					_velocity += glm::vec3(randX, randY, 0);
				}
			break;
		case Blink:
			break;
		case Rupture:
			MoveTowardsClosestPlayer(players);
			if (!_closestPlayer)
				if (_velocity == glm::vec3(0.0))
				{
					float max = 2;
					float randX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max)) - 1.0f;
					float randY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max)) - 1.0f;
					_velocity += glm::vec3(randX, randY, 0);
				}
			break;
			break;
		case Ticker:
			break;
		case Shrapnel:

			if (_velocity == glm::vec3(0.0))
			{
				float max = 2;
				float randX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max)) - 1.0f;
				float randY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max)) - 1.0f;
				_velocity += glm::vec3(randX, randY, 0);
			}
			break;
		case Undefined:
			break;
		};
	}
	setTrapped(false);
}

void AI::MoveTowardsClosestPlayer(const std::vector<std::shared_ptr<Player>>& players)
{
	std::shared_ptr<Player> closestPlayer;
	std::shared_ptr<Player> playerInSafeZone;
	float closestDistance = std::numeric_limits<float>::max();
	for (auto &i : players)
	{
		if (!i->alive())
			continue;

		float distance = glm::length(glm::abs(_Position - i->getPosition()));
		if (closestDistance > distance)
		{
			if (i->PlayerInSafeZone())
			{
				playerInSafeZone = i;
			}

			closestDistance = distance;
			closestPlayer = i;
		}
	}

	if (closestPlayer)
	{
		_velocity = glm::vec3(0.0);
		if (closestPlayer != _closestPlayer)
		{
			_closestPlayer = closestPlayer;
			_targetPosition = closestPlayer->getPosition();
		}
		glm::vec3 dir = closestPlayer->getPosition() - this->getPosition();
		dir = glm::normalize(dir);

		if (closestPlayer == playerInSafeZone)
		{
			_velocity = -_velocity;
		}
		else
		{
			_velocity += dir * _MovementSpeed;
		}

	}
	else
		_closestPlayer = nullptr;
}

void AI::OnCollide(const CollisionData& collisionData)
{
	// Handle collision response here
	//std::cout << "Collided" << std::endl;
	//_collided = true;

	if (const auto bullet = std::dynamic_pointer_cast<Bullet>(collisionData.CollidedObjects[1]))
	{
		_Health -= bullet->getDamage();
		bullet->alive(false);
		if (_Health <= 0)
		{
			alive(false);
			if (_aiBehaviour == Rupture)
			{
				if (const auto gameScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
				{
					gameScene->SpawnAIAtPosition(this->getPosition());
					gameScene->SpawnAIAtPosition(this->getPosition());
					gameScene->SpawnAIAtPosition(this->getPosition());
				}
			}

			const auto standardGame = std::dynamic_pointer_cast<StandardGameScene>(SceneManager::ActiveScene());

			if (standardGame)
			{
				const auto player = standardGame->GetPlayerByID(bullet->getPlayerID());
				if (player)
				{
					player->addScore(_ScoreValue);
					standardGame->GetParticleSystemManager()->AddParticleSystem(EmitterType::Explosion, 0.5f, _Position, glm::vec3(), player->getColour());
					AudioManager::PlaySound("Assets/Audio/AIDeath.wav");		
				}
			}
		}
	}


	if (const auto player = std::dynamic_pointer_cast<Player>(collisionData.CollidedObjects[1]))
	{
		if (player->getShielded())
		{
			alive(false);
			player->setShielded(false);
		}
		else
			player->Damage(_Damage * SceneManager::GetDeltaTime());
	}
}
