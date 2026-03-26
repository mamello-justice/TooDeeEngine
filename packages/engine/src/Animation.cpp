#include "Animation.hpp"

#include <string>
#include <SFML/Graphics.hpp>

#include "Assets.hpp"

Animation::Animation(const std::string& name, const std::string& textureName) : Animation(name, textureName, 1, 0) {}

Animation::Animation(const std::string& name, const std::string& textureName, size_t frameCount, size_t speed)
	: m_name(name),
	m_frameCount(frameCount),
	m_currentFrame(0),
	m_speed(speed),
	m_textureName(textureName) {
	const sf::Texture& t = Assets::Instance().getTexture(textureName);

	m_spriteSize = Vec2f(t.getSize().x / frameCount, t.getSize().y);

	m_sprite = std::make_shared<sf::Sprite>(
		Assets::Instance().getTexture(m_textureName),
		sf::IntRect({ 0, 0 }, { int(m_spriteSize.x), int(m_spriteSize.y) }));
}

void Animation::updateSprite() {}

// updates the animation to show the next frame, depending on its
// animation loops when it reaches the end
void Animation::update() {
	m_currentFrame++;

	if (!m_speed) return;

	auto animFrame = (m_currentFrame / m_speed) % m_frameCount;
	m_sprite->setTextureRect(sf::IntRect({ int(animFrame) * int(m_spriteSize.x), 0 }, { int(m_spriteSize.x), int(m_spriteSize.y) }));
}

bool Animation::hasEnded() const {
	if (!m_speed) return false;
	return (m_currentFrame / m_speed) > m_frameCount;
}

const std::string& Animation::getName() const {
	return m_name;
}

const Vec2f& Animation::getSize() const {
	return m_spriteSize;
}

std::shared_ptr<sf::Sprite> Animation::getSprite() {
	return m_sprite;
}

static_assert(std::is_default_constructible_v<Animation>);
