#pragma once
#include "PostProcessingEffect.h"


class NebulaPostProcess :
	public PostProcessingEffect
{
public:
	NebulaPostProcess(const std::shared_ptr<RenderTarget>& renderTarget, const std::shared_ptr<Shader>& shader);
	virtual ~NebulaPostProcess();
	float RandDecimal();
	void Run(std::shared_ptr<Texture> prevOutput, std::shared_ptr<Graphics> graphics, float dt) override;

private:
	float _time = 0;
	glm::vec3 _currentCol1{};
	glm::vec3 _currentCol2{};
	glm::vec3 _targetCol1{};
	glm::vec3 _targetCol2{};
	glm::vec2 _currentPosition{};
	glm::vec2 _targetPosition{};


	float col1LerpVal = 0;
	float col2LerpVal = 0;
	float posLerpVal = 0;

	float changeTime = 15;
};

