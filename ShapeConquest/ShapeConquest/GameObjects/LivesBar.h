#pragma once
#include "CompositeObject.h"
class LivesBar final : public CompositeObject
{
public:

	LivesBar(std::shared_ptr<int> numLives, std::shared_ptr<int> score, std::shared_ptr<std::vector<wstring>> textureNames, glm::vec3 pos, int maxLives, float scale, glm::vec4 backingColour, const std::string & playerName);
	virtual ~LivesBar();

	LivesBar(const LivesBar&) = delete;
	LivesBar& operator=(const LivesBar&) = delete;
	LivesBar(LivesBar&&) = delete;
	LivesBar& operator=(const LivesBar&&) = delete;

	void Update(const double dt) override;
	void Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera) override;

	void ChangeHealthPointer(const std::shared_ptr<int>& health) { _numLives = health; }
	void ChangeScorePointer(const std::shared_ptr<int>& score) { _score = score; }
	void ChangeTextureNames(const std::shared_ptr<std::vector<wstring>>& texNames) { _textureNames = texNames; }


private:
	std::shared_ptr<int> _numLives;
	std::shared_ptr<int> _score;
	std::shared_ptr<std::vector<std::wstring>> _textureNames;
	int _oldLives;
	int _maxLives;
	int _oldScore;
	std::vector<std::wstring> _oldTextureName;
	float _scale;
};

