#pragma once
#include "CollidableObject.h"


class PowerUp : public CollidableObject
{
public:
	PowerUp(bool screenBound,float radius, glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec4 color,std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc = {});
	virtual ~PowerUp();

	virtual void Update(const double dt) override;

	virtual void OnCollide(const CollisionData& collisionData) override;

	float time = 0;
	glm::vec3 _PreviousScaleDelta;

	const ShootMode& GetShootMode() const { return _ShootMode; }
	void SetShootMode(const ShootMode& shootMode) { _ShootMode = shootMode; }
	const PowerUpType& GetPowerUpType() const { return m_powerUpType; };
	void SetPowerUpType(const PowerUpType& powerUpType)
	{
		m_powerUpType = powerUpType;
		SetColorByType();
	};

	void SetColorByType()
	{
		switch (m_powerUpType)
		{
		case PowerUpType::GravityBomb:
			setColour(glm::vec4(0.38,0.0,0.8,1.0));
			break;
		case PowerUpType::Shield:
			setColour(glm::vec4(1.0,1.0,0.0,1.0));
			break;
		case PowerUpType::ShootMode:
			setColour(glm::vec4(0.92,0.3,0.1,1.0));
			break;
		case PowerUpType::Speed:
			setColour(glm::vec4(0.23,0.59,1.0,1.0));
			break;
		}
	}

	std::string getTextureName() const { return _powerUpTexture; }
	void setTextureName(const std::string& name) { _powerUpTexture = name; }

private:
	PowerUpType m_powerUpType;
	ShootMode _ShootMode = ShootMode::SingleShot;
	std::string _powerUpTexture = "Empty.png";
	
};
