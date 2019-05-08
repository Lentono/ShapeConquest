#pragma once
#include "Texture.h"
#include "Graphics.h"
#include <memory>

//B.M
//
//A Depth stencil  to be written to and used
//
class DepthStencil :Texture
{
public:
	DepthStencil();
	virtual ~DepthStencil();
	
	DepthStencil(const DepthStencil&) = delete;
	DepthStencil& operator=(const DepthStencil&) = delete;
	DepthStencil(DepthStencil&&) = delete;
	DepthStencil& operator=(const DepthStencil&&) = delete;

	void Clear(std::shared_ptr<Graphics>& graphics);//Clear depth stencil

#ifdef DIRECT_X
public:

	DepthStencil(ID3D11Texture2D* const depthStencilBuffer, ID3D11DepthStencilView* const depthStencilView, ID3D11ShaderResourceView* const textureView);
	ID3D11DepthStencilView* DepthStencilView()const { return _depthStencilView; }
private:
	ID3D11DepthStencilView* _depthStencilView;
#endif
};

