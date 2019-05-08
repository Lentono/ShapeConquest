#pragma once
#include "MoveableObject.h"
#include "Player.h"

enum AIBehaviours
{
	Rook, // Maybe change to pawn? NOOOOOO, IT'S JUST A NAME REEEE
	Shuriken,
	Blink,
	Rupture,
	Ticker,
	Shrapnel,
	Undefined
};
class AI : public MoveableObject
{
public:
	AI(bool screenBound, float radius, glm::vec3 position, glm::quat rotation, glm::vec3 scale,glm::vec4 color, std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc, AIBehaviours aiBehaviour = Undefined);
	virtual ~AI();

	float getHealth() const { return _Health; }
	void setHealth(const float health) { _Health = health;}

	AIBehaviours getBehaviour() const { return _aiBehaviour; }
	void setBehaviour(const AIBehaviours& behaviour) { _aiBehaviour = behaviour; }

	void Reset() { _Health = 100.0f; }

	virtual void Update(const double dt) override;
	virtual  void UpdateAI(const std::vector<std::shared_ptr<Player>>& players, const double dt);

	void MoveTowardsClosestPlayer(const std::vector<std::shared_ptr<Player>>& players);

	virtual void OnCollide(const CollisionData& collisionData) override;

	void setTrapped(const bool state) { _trapped = state; }
	bool getTrapped() const { return _trapped; }
	
private:

	AIBehaviours _aiBehaviour = Undefined;
	glm::vec3 _targetPosition;
	std::shared_ptr<Player> _closestPlayer;
	bool _trapped = false;
	float _Health = 100.0f;
	float _Damage = 1.0f;
	float _ScoreValue = 5.0f;
	float _time = 0.0f;
};
