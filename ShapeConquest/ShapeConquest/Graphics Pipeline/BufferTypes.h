#pragma once
#include <glm/glm.hpp>

struct DefaultConstantBuffer
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec4 colour;
};

struct ParticleSystemConstantBuffer 
{
	glm::vec3 direction;
	float totalTime;
	int emitterType;
	float lifeTime;
	glm::vec2 padding;
};

struct SimpleVertex
{
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec3 normal;
};

struct BlurCB
{
	glm::vec2 dimensions;
	int blurDepth;
	float padding;
};

struct NebularCB
{
	float timeMul;
	glm::vec3 nebulaColour;
	float time;
	glm::vec3 backgroundColour;
	glm::vec2 dimensions;
	glm::vec2 vanishingPoint;
};