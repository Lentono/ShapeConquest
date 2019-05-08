#pragma once
#include "BaseScene.h"
#include "../Graphics Pipeline/BufferTypes.h"
#include "../Utilities/AssetFactory.h"
#include "../GameObjects/ButtonObject.h"

class MenuScene final :
	public BaseScene 
{
public:
	MenuScene(int screenWidth, int screenHeight,const float screenBoundsX, const float screenBoundsY);
	virtual ~MenuScene();
	void OnLoad() override;
	void OnUpdateFrame(double deltaTime) override;
	void OnRenderFrame(const std::shared_ptr<Renderer>& renderer, float dt) override;
	void Reset() override;
	void Shutdown() override;

	void OnSwitchIn() override;
	void OnSwitchOut() override;
private:

	std::shared_ptr<GameObject> _bigBoss;
};

