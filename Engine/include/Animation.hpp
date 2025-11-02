#pragma once

#include <string>
#include <SFML/Graphics.hpp>

#include "Assets.hpp"

class Animation
{
	/**
	 * @brief Total number of frames in the animation
	 */
	size_t m_frameCount = 1;
	/**
	 * @brief The current frame of animation being played
	 */
	size_t m_currentFrame = 0;
	/**
	 * @brief Speed to play the animation
	 */
	size_t m_speed = 0;
	/**
	 * @brief Name of the animation
	 */
	std::string m_name = "none";
	/**
	 * @brief Name of the texture to get the frames from
	 */
	std::string m_textureName = "none";
	/**
	 * @brief Sub-rectangle to draw
	 */
	sf::IntRect m_textureRect;

	std::shared_ptr<sf::Sprite> m_sprite;

	void updateSprite();

public:
	/**
	 * @brief Default constructor
	 */
	Animation() = default;

	/**
	 * @brief Parameterized constructor
	 *
	 * @param name The name of the animation
	 * @param textureName The name of the texture to get frames from
	 */
	Animation(const std::string &name, const std::string &textureName);

	/**
	 * @brief Parameterized constructor
	 *
	 * @param name The name of the animation
	 * @param textureName The name of the texture to get frames from
	 * @param frameCount Total number of frames in the animation
	 * @param speed Speed to play the animation
	 */
	Animation(const std::string &name, const std::string &textureName, size_t frameCount, size_t speed);

	/**
	 * @brief Update the animation to show the next frame
	 */
	void update();

	/**
	 * @brief Check if the animation has ended
	 */
	bool hasEnded() const;

	/**
	 * @brief Get the name of the animation
	 *
	 * @return The name of the animation
	 */
	const std::string &getName() const;

	/**
	 * @brief Get the texture rectangle of the current frame
	 *
	 * @return The texture rectangle
	 */
	const sf::IntRect &getRect() const;

	/**
	 * @brief Get the sprite for the current frame
	 *
	 * @return The sprite
	 */
	std::shared_ptr<sf::Sprite> getSprite();
};

static_assert(std::is_default_constructible_v<Animation>);
