#pragma once
#include "ShaderEnum.h"
#include "Camera.h"
#include <string>
#include <vector>

class RenderDescription final
{
public:
	explicit RenderDescription(ShaderEnum shader = ShaderEnum::Default, Camera::Types projectionType = Camera::Perspective, std::string cameraReference = "Default", std::vector<std::string> renderTargets = {});
	~RenderDescription();

	RenderDescription(const RenderDescription&) = delete;
	RenderDescription& operator=(const RenderDescription&) = delete;
	RenderDescription(RenderDescription&&) = delete;
	RenderDescription& operator=(const RenderDescription&&) = delete;


	const std::vector<std::string>& RenderTargets() const { return _renderTargets; }
	ShaderEnum ShaderToUse() const { return _shader; }
	Camera::Types ProjectionType() const { return _projectionType; }
	const std::string& CameraReferrence() const { return _cameraRefference; }

private:
	std::vector<std::string> _renderTargets{};//Which render targets the object wants to be drawn to, if left empty will render to back buffer
	ShaderEnum _shader;
	Camera::Types _projectionType;
	std::string _cameraRefference;
};

