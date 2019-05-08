#pragma once
#include <string>
#include <glm/glm.hpp>
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\>
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_>
//	Class for describing deferred text render.
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_>
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\>
class TextDescription
{
public:
	TextDescription(std::string text, glm::vec2 screenPosition, glm::vec4 colour, glm::vec2 scale, float wrapWidth);
	~TextDescription();

	//TextDescription(const TextDescription&) = delete;
	//TextDescription& operator=(const TextDescription&) = delete;
	//TextDescription(TextDescription&&) = delete;
	//TextDescription& operator=(const TextDescription&&) = delete;

	const std::string& Text() const { return _text; }
	const glm::vec2& ScreenPosition() const { return _screenPosition; }
	const glm::vec4& Colour()const { return _colour; }
	float WrapWidth()const { return _wrapWidth; }
	glm::vec2 Scale()const { return _scale; }

	void SetScale(glm::vec2 newScale) { _scale = newScale; }

	void SetText(const std::string& newText) { _text = newText; }
	void SetColour(glm::vec4 newColour) { _colour = newColour; }

private:
	std::string _text;
	glm::vec2 _screenPosition;
	glm::vec4 _colour;
	float _wrapWidth;
	glm::vec2 _scale;
};

