#include "ParticleManager.h"
#include "SceneManager.h"
#include "../GameScene.h"


ParticleManager::ParticleManager() : m_particleSystems()
{
}


ParticleManager::~ParticleManager()
{
}

void ParticleManager::AddParticleSystem(EmitterType emitterType, const float lifetime, const glm::vec3& position, const glm::vec3& direction, const glm::vec4& colour)
{
	const auto particleSystem = std::make_shared<ParticleSystem>(emitterType, position, colour, 100, 1.0f, std::make_shared<RenderDescription>(ShaderEnum::ParticleSystem));

	particleSystem->SetLifeTime(lifetime);

	if (direction != glm::vec3())
	{
		particleSystem->SetMovementDirection(direction);
	}

	m_particleSystems.emplace_back(particleSystem);

	if (const auto& gameScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
	{
		gameScene->AddGameObject(particleSystem);
	}
}

void ParticleManager::Update(const double dt)
{
	//Particle System lifetime check
	for (int i = m_particleSystems.size() - 1; i >= 0; i--)
	{
		auto& particleSystem = m_particleSystems[i];

		if (particleSystem->GetTotalTime() > particleSystem->GetLifeTime())
		{
			particleSystem->alive(false);
			
			if (const auto standardScene = std::dynamic_pointer_cast<GameScene>(SceneManager::ActiveScene()))
			{
				standardScene->RemoveGameObject(particleSystem);
				particleSystem = nullptr;

				m_particleSystems.erase(m_particleSystems.begin() + i);
			}
		}
	}
}

void ParticleManager::Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera)
{
	
}
