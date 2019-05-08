#pragma once
#include "Scenes/BaseScene.h"

class PauseScene :	public BaseScene
{
public:
	PauseScene(const int screenWidth, const int screenHeight, const float screenBoundsX, const float screenBoundsY);
	~PauseScene();

	virtual void OnLoad() override;


	virtual void OnUpdateFrame(double deltaTime) override;


	virtual void OnRenderFrame(const std::shared_ptr<Renderer>& renderer, float dt) override;


	virtual void Reset() override;


	virtual void Shutdown() override;


	virtual void OnSwitchIn() override;


	virtual void OnSwitchOut() override;


	virtual void AddGameObject(const std::shared_ptr<Object>& gameObject) override;


	virtual void RemoveGameObject(const std::shared_ptr<Object>& gameObject) override;

};

