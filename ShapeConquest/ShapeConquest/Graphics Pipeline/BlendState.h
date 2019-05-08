#pragma once
#include "APIDeffinition.h"
#include <vector>
#include <memory>
#include "Graphics.h"

#ifdef DIRECT_X
#include <d3d11.h>
#endif
//B.M
//
//Defines a blend state for a render
//
class BlendState
{
public:
	BlendState();
	~BlendState();

	BlendState(const BlendState&) = delete;
	BlendState& operator=(const BlendState&) = delete;
	BlendState(BlendState&&) = delete;
	BlendState& operator=(const BlendState&&) = delete;

	void Load(const std::shared_ptr<Graphics>& renderer);//Set this as the current blend state

#ifdef DIRECT_X
public:
	BlendState(ID3D11BlendState* blend_state, std::vector<float> blendFactors = {1,1,1,1}, unsigned int sampleMask = 0xffffff);
private:
	ID3D11BlendState* _blendState = nullptr;
	std::vector<float> _blendFactors;
	unsigned int _sampleMask = 0xffffff;
#endif

};

