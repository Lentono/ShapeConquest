#include "PowerUp.h"



PowerUp::PowerUp(bool screenBound, float radius, glm::vec3 position, glm::quat rotation, glm::vec3 scale,glm::vec4 color,
	std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc)
	: CollidableObject(screenBound, radius, position, rotation, scale, color,geometry, rendDesc)
{
	_entityType = static_cast<short>(EntityCategory::PowerUp);
	_collisionMask = static_cast<short>(EntityCategory::Player);
}


PowerUp::~PowerUp()
{
}

void PowerUp::Update(const double dt)
{
	CollidableObject::Update(dt);
	time += dt;


	const auto amount = glm::sin(time * 2) + 1.0;
	const auto delta = (glm::vec3(amount * 0.5)*0.05f);
	_PreviousScaleDelta = delta;
	_Scale += delta - _PreviousScaleDelta;

	const float angleChange = 1.0f * dt;
	const auto newYQuat = glm::angleAxis(angleChange, glm::vec3(0, 0, 1));

	_Rotation *= newYQuat;
}

void PowerUp::OnCollide(const CollisionData& collisionData)
{
	// Handle collision response here
	//std::cout << "Collided" << std::endl;
}
