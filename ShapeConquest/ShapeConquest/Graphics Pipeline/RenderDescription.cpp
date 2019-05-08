#include "RenderDescription.h"


RenderDescription::RenderDescription(ShaderEnum shader, Camera::Types projectionType, std::string cameraReference, std::vector<std::string> renderTargets)
	: _renderTargets(std::move(renderTargets)), _shader(shader), _projectionType(projectionType), _cameraRefference(std::move(cameraReference))
{
	if(_renderTargets.empty())
	{
		_renderTargets.push_back("Default");
	}
}

RenderDescription::~RenderDescription()
= default;
