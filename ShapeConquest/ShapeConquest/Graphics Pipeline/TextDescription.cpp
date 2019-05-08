#include "TextDescription.h"


TextDescription::TextDescription(std::string text, glm::vec2 screenPosition, glm::vec4 colour, glm::vec2 scale, float wrapWidth)
	: _text(std::move(text)), _screenPosition(screenPosition), _colour(colour), _wrapWidth(wrapWidth), _scale(scale)
{
}

TextDescription::~TextDescription()
= default;
