#pragma once
#include "APIDeffinition.h"
#include "Graphics.h"
#include <memory>

#ifdef DIRECT_X
//DirectX
#include <d3d11_1.h>
#endif
//B.M
//
//Defines the way a depth stencil is used
//
class DepthState
{
public:
	DepthState();
	~DepthState();

	DepthState(const DepthState&) = delete;
	DepthState& operator=(const DepthState&) = delete;
	DepthState(DepthState&&) = delete;
	DepthState& operator=(const DepthState&&) = delete;

	void SetState(std::shared_ptr<Graphics>& graphics) const;//Use as current depth state

#ifdef DIRECT_X
public:
	explicit DepthState(ID3D11DepthStencilState* const depthStencilState);
private:
	ID3D11DepthStencilState* _depthStencilState;
#endif

};

