#pragma once

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

#include "EntityManager.hpp"
#include "GameEngine.hpp"
#include "Scene.hpp"
#include "Vec2.hpp"

class Scene_Play : public Scene
{
	struct PlayerConfig
	{
		float X, Y,
			CW, CH,
			SX, SY, SM,
			GY;
		std::string B;
	};

protected:
	std::shared_ptr<Entity> m_player;
	std::string m_levelPath;
	PlayerConfig m_playerConfig;
	bool m_drawTextures = true;
	bool m_drawCollision = false;
	bool m_drawGrid = false;
	bool m_drawAnimName = false;
	const Vec2f m_gridSize = { 64, 64 };
	sf::Text m_gridText;

	void init(const std::string& levelPath);
	void update() override;

	void loadLevel(const std::string& filename);

	void onEnd();

	void sAnimation();
	void sCollision();
	void sDoAction(const Action& action) override;
	void sEnemySpawner();
	void sLifespan();
	void sMovement();
	void sRender() override;

public:
	Scene_Play(std::shared_ptr<GameEngine> gameEngine, const std::string& levelPath);

	Vec2f gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
	void spawnPlayer();
	void spawnBullet(std::shared_ptr<Entity> entity);

	void addTile(const std::string& animName, const Vec2f& gridPos);
	void addDecoration(const std::string& animName, const Vec2f& gridPos);

	void onFrame();
};
