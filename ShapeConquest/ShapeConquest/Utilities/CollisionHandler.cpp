#include "CollisionHandler.h"



bool CollisionHandler::SphereSphereCollisionCheck(const std::shared_ptr<CollidableObject>& c1, const std::shared_ptr<CollidableObject>& c2, CollisionData& collisionData)
{
	const auto c1Pos = c1->getPosition();
	const auto c2Pos = c2->getPosition();

	const auto midline = c1Pos - c2Pos;

	const auto distance = glm::abs(glm::length(midline));

	const auto c1Radius = c1->getRadius();
	const auto c2Radius = c1->getRadius();

	if (distance < c1Radius + c2Radius && distance >= 0)
	{
		// Collided
		collisionData.CollidedObjects[0] = c1;
		collisionData.CollidedObjects[1] = c2;

		const auto normal = midline * (1 / distance);

		collisionData.ContactNormal = normal;

		// Calculate collision contact point
		const auto x = c1Pos.x + (((c2Pos.x - c1Pos.x) * c1Radius) / (c1Radius + c2Radius));
		const auto y = c1Pos.y + (((c2Pos.y - c1Pos.y) * c1Radius) / (c1Radius + c2Radius));
		const auto z = c1Pos.z + (((c2Pos.z - c1Pos.z) * c1Radius) / (c1Radius + c2Radius));

		collisionData.CollisionPoint = glm::vec3(x, y, z);
		return true;
	}

	return false;
}

bool CollisionHandler::SquareSphereCollisionCheck(const std::shared_ptr<CollidableObject>& c, const std::shared_ptr<BoxCollider> & b)
{
	const auto cPos = c->getPosition();
	const auto bPos = b->getCentre();

	float xDist = glm::abs(cPos.x - bPos.x);
	float yDist = glm::abs(cPos.y - bPos.y);

	return (xDist < b->getWidth() + c->getRadius() && yDist < b->getHeight() + c->getRadius());
}

bool CollisionHandler::ScreenBoundCheck(const std::shared_ptr<CollidableObject>& c, const float screenWidth, const float screenHeight, glm::vec3& collisionVector)
{
	if(screenWidth == 0 || screenHeight == 0)
		return false;

	const auto position = c->getPosition();
	const auto radius = c->getRadius();

	if (position.x < -(screenWidth) + radius)
	{
		collisionVector = glm::vec3(1,0,0);
		return true;
	}
	else if (position.y < -(screenHeight) + radius)
	{
		collisionVector = glm::vec3(0,1,0);
		return true;
	}
	else if (position.x > (screenWidth) - radius)
	{
		collisionVector = glm::vec3(-1,0,0);
		return true;
	}
	else if (position.y > (screenHeight) - radius)
	{
		collisionVector = glm::vec3(0,-1,0);
		return true;
	}

	return false;
}
