#pragma once
#include "GameObject.h"
#include "../Utilities/AssetFactory.h"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum class EmitterType 
{
	Straight,
	Cone,
	Explosion,
	Spiral
};

class ParticleSystem : public GameObject
{
public:
	ParticleSystem(EmitterType emitterType, const glm::vec3& position, const glm::vec4& colour, const int particleCount, const float density, std::shared_ptr<RenderDescription> rendDesc);
	ParticleSystem(const ParticleSystem& other) = delete;
	ParticleSystem(ParticleSystem&& other) = delete;
	~ParticleSystem();

	ParticleSystem& operator = (const ParticleSystem& other) = delete;
	ParticleSystem& operator = (ParticleSystem&& other) = delete;

	float GetTotalTime() const;

	float GetLifeTime() const;
	void SetLifeTime(const float lifeTime);



	const glm::vec3& GetMovementDirection() const;
	void SetMovementDirection(const glm::vec3& movementDirection);

	void Update(const double dt) override;
	void Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera) override;

private:
	glm::quat FindRotationBetweenVectors(glm::vec3 start, glm::vec3 destination);

	EmitterType m_emitterType;

	const glm::vec3& m_referencePosition;
	glm::vec3 m_previousPosition;
	glm::vec3 m_movementDirection;

	glm::vec4 m_colour;

	int m_particleCount;

	float m_density;
	float m_lifeTime;
	float m_totalTime;
};

