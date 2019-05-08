#pragma once
#include "PostProcessingEffect.h"
class PassOverPostProcess final :
	public PostProcessingEffect
{
public:
	PassOverPostProcess(const std::shared_ptr<RenderTarget>& renderTarget, const std::shared_ptr<Shader>& shader);
	virtual ~PassOverPostProcess();
	void Run(std::shared_ptr<Texture> prevOutput, std::shared_ptr<Graphics> graphics, float dt) override;
};

