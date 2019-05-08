#pragma once

#include "../GameObjects/ParticleSystem.h"

#include <vector>

class ParticleManager : public Object
{
public:
	ParticleManager();
	ParticleManager(const ParticleManager& other) = delete;
	ParticleManager(ParticleManager&& other) noexcept = delete;
	~ParticleManager();

	ParticleManager& operator = (const ParticleManager& other) = delete;
	ParticleManager& operator = (ParticleManager&& other) noexcept = delete;

	void AddParticleSystem(EmitterType emitterType, const float lifetime, const glm::vec3& position, const glm::vec3& direction, const glm::vec4& colour);

	void Update(const double dt) override;
	void Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera) override;

private:
	std::vector<std::shared_ptr<ParticleSystem>> m_particleSystems;
};

