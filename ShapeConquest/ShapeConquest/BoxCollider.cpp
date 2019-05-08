#include "BoxCollider.h"
#include "Utilities/CollisionHandler.h"


BoxCollider::BoxCollider(float width, float height, const glm::vec3& centre, float minSize) : _Width(width), _Height(height), _Centre(centre)
{
	if (_Width < minSize || height < minSize)
		return;
	std::vector<glm::vec3> centres;
	centres.push_back(glm::vec3(_Centre.x - (_Width / 4), _Centre.y - (_Height / 4), 0));
	centres.push_back(glm::vec3(_Centre.x - (_Width / 4), _Centre.y + (_Height / 4), 0));
	centres.push_back(glm::vec3(_Centre.x + (_Width / 4), _Centre.y - (_Height / 4), 0));
	centres.push_back(glm::vec3(_Centre.x + (_Width / 4), _Centre.y + (_Height / 4), 0));

	for (auto i = 0; i < 4; i++)
	{
		auto boxCollider = std::make_shared<BoxCollider>(width / 2, height / 2, centres.at(i), minSize);
		_SubColliders.push_back(boxCollider);
	}
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::HandleCollision()
{
	if(_SubColliders.empty())
	{
		if(_Objects.size() > 1)
		{
			for (int i = 0; i < _Objects.size(); ++i)
			{
				if (i <= _Objects.size() - 1 && _Objects.at(i)->alive())
				{
					for (int j = i + 1; j < _Objects.size(); ++j)
					{
						if (_Objects.at(j)->alive())
						{

							// Stores data related to current collision
							CollisionData collide;
							auto collidableI = std::static_pointer_cast<CollidableObject>(_Objects.at(i));
							auto collidableJ = std::static_pointer_cast<CollidableObject>(_Objects.at(j));

							//if (collidableI->getEntityType() == static_cast<short>(EntityCategory::PlayerZone))
							//{
							//	auto test = false;
							//}

							//if (collidableJ->getEntityType() == static_cast<short>(EntityCategory::PlayerZone))
							//{
							//	auto test = false;
							//}

							short result1 = (collidableI->getCollisionMask() & collidableJ->getEntityType());
							short result2 = (collidableI->getEntityType() & collidableJ->getCollisionMask());
							if (result1 != 0 && result2 != 0)
							{
								if (CollisionHandler::SphereSphereCollisionCheck(collidableI, collidableJ, collide))
								{
									// Notify objects of collision
									collidableI->OnCollide(collide);
									collide.CollidedObjects[0] = collidableJ;
									collide.CollidedObjects[1] = collidableI;
									collidableJ->OnCollide(collide);
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		for(auto &i: _SubColliders)
		{
			i->HandleCollision();
		}
	}
}

void BoxCollider::Remove(const std::shared_ptr<Object>& toRemove)
{
	for (int i = 0; i < _Objects.size(); i++)
	{
		if (_Objects.at(i) == toRemove)
		{
			_Objects.erase(_Objects.begin() + i);
			return;
		}
	}
}

void BoxCollider::Add(const std::shared_ptr<CollidableObject>& toAdd)
{
	if (!_SubColliders.empty())
	{
		for (auto &i : _SubColliders)
		{

			if (CollisionHandler::SquareSphereCollisionCheck(toAdd, i))
			{
				i->Add(toAdd);
				
			}
		}
	}
	else
	{
		_Objects.push_back(toAdd);
		toAdd->AddCollisionCell(shared_from_this());
	}
}

