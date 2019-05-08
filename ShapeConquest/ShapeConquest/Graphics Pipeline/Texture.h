#pragma once
#include "APIDeffinition.h"
#ifdef DIRECT_X
#include <d3d11_1.h>
#endif
#include <exception>
//B.M
//
//Texture to be used as a store of pixels to be used in shading or rendering to
//
class Texture
{
public:
	Texture();
	virtual ~Texture();
	
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&&) = delete;
	Texture& operator=(const Texture&&) = delete;

#ifdef DIRECT_X
public:
	Texture(ID3D11Resource* const texture, ID3D11ShaderResourceView* const textureView);

	Texture(ID3D11Resource* const texture, ID3D11ShaderResourceView* const textureView,
		ID3D11RenderTargetView* const renderTarget);
	ID3D11Resource* Get()const { return _texture; }
	ID3D11ShaderResourceView* GetView()const { return _textureView; }
	ID3D11RenderTargetView* GetRenderTarget()const { return _renderTarget; }
protected:
	ID3D11Resource* _texture;
	ID3D11ShaderResourceView* _textureView;
	ID3D11RenderTargetView* _renderTarget;
#endif

};

