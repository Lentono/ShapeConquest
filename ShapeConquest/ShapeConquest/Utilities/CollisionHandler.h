#pragma once
#include <memory>
#include "../GameObjects/CollidableObject.h"
#include "../BoxCollider.h"

class CollisionHandler
{
public:

	static bool SphereSphereCollisionCheck(const std::shared_ptr<CollidableObject>& c1, const std::shared_ptr<CollidableObject>& c2, CollisionData& collisionData);
	static bool SquareSphereCollisionCheck(const std::shared_ptr<CollidableObject>& c, const std::shared_ptr<BoxCollider> & b);
	static bool ScreenBoundCheck(const std::shared_ptr<CollidableObject>& c, const float screenWidth, const float screenHeight,  glm::vec3& collisionVector);
};

