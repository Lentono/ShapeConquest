#include "GravityBomb.h"
#include "GameObjects/AI.h"
#include "GameObjects/ParticleSystem.h"
#include "Managers/SceneManager.h"


GravityBomb::GravityBomb(bool screenBound, float radius, glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec4 color,std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc) : CollidableObject(screenBound, radius, position, rotation, scale, color,geometry, rendDesc)
{
	_PreviousScaleDelta = glm::vec3(0);
	_entityType = static_cast<glm::uint16>(EntityCategory::Bomb);
	_collisionMask = static_cast<glm::uint16>(EntityCategory::AI);
}


GravityBomb::~GravityBomb()
{
}

void GravityBomb::Update(const double dt)
{
	CollidableObject::Update(dt);
	if (_Triggered && _GravityPull != 0)
	{
		if (_GravityPull > 0)
			_GravityPull -= 1.0 * dt;

		time += dt;


		const auto amount = glm::sin(time * 5) + 1.0;
		const auto delta = (glm::vec3(amount * 0.5f)*0.1f);		
		_Scale += delta - _PreviousScaleDelta;
		_PreviousScaleDelta = delta;

		const float angleChange = 10.0f * dt;
		const auto newYQuat = glm::angleAxis(angleChange, glm::vec3(0, 0, 1));

		_Rotation *= newYQuat;

	}
	if(_GravityPull <=0)
		alive(false);

}

void GravityBomb::Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera)
{
	CollidableObject::Render(graphics, shader, camera);
}

void GravityBomb::OnCollide(const CollisionData& collisionData)
{
	
	const auto direction = collisionData.CollidedObjects[0]->getPosition() - collisionData.CollidedObjects[1]->getPosition();
	if (const auto ai = std::dynamic_pointer_cast<AI>(collisionData.CollidedObjects[1]))
	{
		if (!_Triggered)
		{
			_Triggered = true;

			if (const auto standardGameScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
			{
				standardGameScene->GetParticleSystemManager()->AddParticleSystem(EmitterType::Spiral, 4.0f, _Position, glm::vec3(), _Colour);
			}
		}
		if (alive())
		{
			const auto velocity = ai->getVelocity();
			ai->setTrapped(true);
			ai->setVelocity((direction * _GravityPull));
		}
		else
			ai->setTrapped(false);
	}
}
