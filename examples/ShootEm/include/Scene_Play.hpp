#pragma once

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

#include "Core.hpp"
#include "EntityManager.hpp"
#include "GameEngine.hpp"
#include "Scene.hpp"

class Scene_Play : public Scene
{
protected:
	sf::Text m_scoreText;
	std::shared_ptr<Entity> m_player;
	bool m_drawTextures = true;
	bool m_drawCollision = false;
	bool m_drawGrid = false;
	const Vec2f m_gridSize = { 64, 64 };
	int m_score = 0;
	int m_currentFrame = 0;
	int m_lastEnemySpawnTime = 0;
	bool m_paused = false;

	bool m_isMovementActive = true;
	bool m_isUserInputActive = true;
	bool m_isLifespanActive = true;
	bool m_isEnemySpawnerActive = true;
	bool m_isCollisionActive = true;

	void init();
	void update() override;

	void loadLevel();

	void onEnd();

	void sCollision();
	void sClickHandler(const Vec2f& mPos, const sf::Mouse::Button& button) override;
	void sDoAction(const Action& action) override;
	void sEnemySpawner();
	void sLifespan();
	void sMovement();
	void sRender() override;

public:
	Scene_Play(std::shared_ptr<GameEngine> gameEngine);

	Vec2f gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity>);
	void spawnBullet(std::shared_ptr<Entity>, const Vec2f&);
	void spawnSpecialWeapon(std::shared_ptr<Entity>);
};

template void GameEngine::changeScene<Scene_Play>(
	const std::string&, std::shared_ptr<Scene_Play>);
