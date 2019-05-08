#pragma once
#include "BaseScene.h"
class GameOverScene final :
	public BaseScene
{
public:
	GameOverScene(int screenWidth, int screenHeight, const float screenBoundsX, const float screenBoundsY, int highScore, int winningPlayerNumber);
	virtual ~GameOverScene();
	void OnUpdateFrame(double deltaTime) override;
	void OnRenderFrame(const std::shared_ptr<Renderer>& renderer, float dt) override;
	void Reset() override;
	void Shutdown() override;
	void OnSwitchIn() override;
	void OnSwitchOut() override;

	void OnLoad() override;
private :
	int _winningScore;
	int _winningPlayerNumber;
};

