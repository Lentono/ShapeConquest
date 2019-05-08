#pragma once
#include "PostProcessingEffect.h"
class BlurPostProcess :
	public PostProcessingEffect
{
public:
	BlurPostProcess(const std::shared_ptr<RenderTarget>& renderTarget, const std::shared_ptr<Shader>& shader);
	virtual ~BlurPostProcess();
	void Run(std::shared_ptr<Texture> prevOutput, std::shared_ptr<Graphics> graphics, float dt) override;
};

