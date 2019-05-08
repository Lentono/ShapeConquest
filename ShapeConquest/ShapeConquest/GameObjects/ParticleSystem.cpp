#include "ParticleSystem.h"
#include <glm/mat4x4.hpp>


ParticleSystem::ParticleSystem(EmitterType emitterType, const glm::vec3& position, const glm::vec4& colour, const int particleCount, const float density, std::shared_ptr<RenderDescription> rendDesc) :
	GameObject(position, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), nullptr, rendDesc),
		m_emitterType(emitterType), m_referencePosition(position), m_colour(colour), m_previousPosition(glm::vec3()), m_movementDirection(glm::vec3()), m_particleCount(particleCount), m_density(density), m_totalTime(0.0f), m_lifeTime(std::numeric_limits<float>::max())
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> instancePos;

	vertices.emplace_back(glm::vec3());

	const auto particleStepSize = m_density / m_particleCount;

	for (auto i = 0.0f; i < m_density; i = i + particleStepSize)
	{
		instancePos.emplace_back(glm::vec3(0.0f, 0.0f, i));
	}

	const std::vector<short> blank;

	//Create Geometry for particle system
	const std::shared_ptr<Geometry> geometry = AssetFactory::CreateGeometry(vertices, instancePos, blank, false, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	SetGeometry(geometry);

	//A Particle System Manager would probably do this instead and just pass the geometry through?

}

ParticleSystem::~ParticleSystem()
{
}

float ParticleSystem::GetTotalTime() const
{
	return m_totalTime;
}

float ParticleSystem::GetLifeTime() const
{
	return m_lifeTime;
}

void ParticleSystem::SetLifeTime(const float lifeTime)
{
	m_lifeTime = lifeTime;
}

const glm::vec3& ParticleSystem::GetMovementDirection() const
{
	return m_movementDirection;
}

void ParticleSystem::SetMovementDirection(const glm::vec3& movementDirection) 
{
	m_movementDirection = movementDirection;

	const glm::vec3 rotationAxis(0.0f, 0.0f, 1.0f);

	const auto dot = 0.0f * m_movementDirection.x + 1.0f * m_movementDirection.y;
	const auto det = 0.0f * m_movementDirection.y - 1.0f * m_movementDirection.x;
	const auto angleRad = atan2(det, dot);

	const auto rotation = glm::angleAxis(angleRad, rotationAxis);
	_RotationMatrix = glm::mat4_cast(rotation);
}

void ParticleSystem::Update(const double dt) 
{
	m_totalTime += static_cast<float>(dt);
}


void ParticleSystem::Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera)  
{
	ParticleSystemConstantBuffer vertexCB{ m_movementDirection, m_totalTime, static_cast<int>(m_emitterType), m_lifeTime, glm::vec2()};

	shader->UpdateBuffer(ShaderTypes::Vertex, &vertexCB, graphics);

	const auto model = translate(glm::mat4(1.0), _Position)  * _RotationMatrix *glm::scale(glm::mat4(1.0), _Scale);
	DefaultConstantBuffer geometryCB{ glm::transpose(model), glm::transpose(camera->View()), glm::transpose(camera->Projection(ProjectionType())), m_colour };

	shader->UpdateBuffer(ShaderTypes::Geometry, &geometryCB, graphics);

	GetGeometry()->Prep(graphics);
	GetGeometry()->Render(graphics);
}