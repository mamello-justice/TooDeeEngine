#pragma once

#include <string>
#include <SFML/Graphics.hpp>

#include "Assets.hpp"
#include "Vec2.hpp"

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
	 * @brief Sprite rect
	 */
	sf::IntRect m_spriteRect;

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
	Animation(const std::string& name, const std::string& textureName);

	/**
	 * @brief Parameterized constructor
	 *
	 * @param name The name of the animation
	 * @param textureName The name of the texture to get frames from
	 * @param frameCount Total number of frames in the animation
	 * @param speed Speed to play the animation
	 */
	Animation(const std::string& name, const std::string& textureName, size_t frameCount, size_t speed);

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
	const std::string& getName() const;

	/**
	 * @brief Get the name of the underlying texture
	 *
	 * @returns The name of the texture
	 */
	const std::string& getTextureName() const;

	/**
	 * @brief Get the sprite rect for the current frame
	 *
	 * @return The sprite rect
	 */
	const sf::IntRect& getSpriteRect() const;
};

static_assert(std::is_default_constructible_v<Animation>);
