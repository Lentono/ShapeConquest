#include "Bullet.h"
#include "GameObjects/ParticleSystem.h"
#include "Managers/SceneManager.h"
#include "GameScene.h"


Bullet::Bullet(const int playerID,const glm::vec2 directionVector, bool screenBound, float radius, const glm::vec3 position, const glm::quat rotation, const glm::vec3 scale, glm::vec4 color,std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc) : MoveableObject(screenBound, radius, position, rotation, scale,color, geometry, rendDesc), _OwningPlayerID(playerID)
{
	_velocity = glm::vec3(directionVector.x, directionVector.y, 0);
	alive(true);
	_entityType = static_cast<short>(EntityCategory::Bullet);
	_collisionMask = static_cast<short>(EntityCategory::AI);
}

Bullet::~Bullet()
{
}


void Bullet::OnCollide(const CollisionData & collisionData)
{
	// if(collisionData.CollidedObjects[1] == Player)
	//{
	//		ParticleSystem::TriggerExplosion(_Position)
	//		Destroy(this)
	//}
}

void Bullet::Update(const double dt)
{
	MoveableObject::Update(dt);

	moved(true);
	_Position += (static_cast<float>(dt) * _bulletSpeed *  _velocity );

	float angleChange = 5.0f * dt;
	glm::quat newYQuat = glm::angleAxis(angleChange, glm::vec3(0, 0, 1));

	m_invertDirection = glm::normalize(_oldPosition - _Position);

	_Rotation *= newYQuat;

	_oldPosition = _Position;
}

void Bullet::ScreenBound(const glm::vec3& collisionVector)
{
	if (alive())
	{
		std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene())->GetParticleSystemManager()->AddParticleSystem(EmitterType::Cone, 0.5f, _Position, m_invertDirection, m_playerColour);
	}
	
	alive(false);
}

