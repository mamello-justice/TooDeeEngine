#include "Scene_Play.hpp"

#include <SFML/Graphics.hpp>

#include "Action.hpp"
#include "Assets.hpp"
#include "Components.hpp"
#include "GameConfig.hpp"
#include "GameEngine.hpp"
#include "Physics.hpp"
#include "Scene_Menu.hpp"

Scene_Play::Scene_Play(std::shared_ptr<GameEngine> gameEngine)
	:
	Scene(gameEngine),
	m_scoreText(Assets::Instance().getFont("orbitron"), "Deafult", 24) {
	init();
}

void Scene_Play::init() {
	registerAction(sf::Keyboard::Scancode::P, "PAUSE");
	registerAction(sf::Keyboard::Scancode::Escape, "QUIT");
	registerAction(sf::Keyboard::Scancode::W, "UP");
	registerAction(sf::Keyboard::Scancode::A, "LEFT");
	registerAction(sf::Keyboard::Scancode::S, "DOWN");
	registerAction(sf::Keyboard::Scancode::D, "RIGHT");
	registerAction(sf::Keyboard::Scancode::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::Scancode::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::Scancode::G, "TOGGLE_GRID");
	registerAction(sf::Keyboard::Scancode::P, "PAUSE");

	// set text position
	m_scoreText.setPosition({ 20.0f, 20.0f });

	// set text font size
	m_scoreText.setCharacterSize(24);

	// set text font color
	m_scoreText.setFillColor({ 255, 255, 255 });

	loadLevel();
}

void Scene_Play::update() {
	m_entityManager.update();

	sEnemySpawner();
	sMovement();
	sLifespan();
	sCollision();
	sRender();

	m_currentFrame++;
}

void Scene_Play::loadLevel() {
	m_entityManager = EntityManager();
	spawnPlayer();
}

void Scene_Play::onEnd() {
	m_gameEngine->changeScene("Scene_Menu", std::make_shared<Scene_Menu>(m_gameEngine));
}

void Scene_Play::sClickHandler(const Vec2f& mPos, const sf::Mouse::Button& button) {
	spawnBullet(m_player, mPos);
}

void Scene_Play::sCollision() {
	if (m_paused) { return; }

	auto wSize = m_gameEngine->window().getSize();

	// Wall Collision
	{
		auto& cTransform = m_player->get<CTransform>();
		auto& cCollision = m_player->get<CBoundingCircle>();

		if (cTransform.pos.y <= cCollision.radius) {
			cTransform.pos.y = cCollision.radius;
		}
		else if (cTransform.pos.y >= wSize.y - cCollision.radius) {
			cTransform.pos.y = wSize.y - cCollision.radius;
		}

		if (cTransform.pos.x <= cCollision.radius) {
			cTransform.pos.x = cCollision.radius;
		}
		else if (cTransform.pos.x >= wSize.x - cCollision.radius) {
			cTransform.pos.x = wSize.x - cCollision.radius;
		}
	}

	for (auto& e : m_entityManager.getEntities("enemy")) {
		if (e->has<CTransform>() && e->has<CBoundingCircle>()) {
			auto& cTransform = e->get<CTransform>();
			auto& cCollision = e->get<CBoundingCircle>();
			// Vertical
			if (cTransform.pos.y <= cCollision.radius and cTransform.velocity.y < 0
				or cTransform.pos.y >= wSize.y - cCollision.radius and cTransform.velocity.y > 0) {
				cTransform.velocity.y *= -1;
			}
			// Horizontal
			if (cTransform.pos.x <= cCollision.radius and cTransform.velocity.x < 0
				or cTransform.pos.x >= wSize.x - cCollision.radius and cTransform.velocity.x > 0) {
				cTransform.velocity.x *= -1;
			}
		}
	}

	if (!m_isCollisionActive) { return; }

	// Bullet Collision
	for (auto& b : m_entityManager.getEntities("bullet")) {
		for (auto& e : m_entityManager.getEntities("enemy")) {
			if (Physics::CircleCollision(b, e)) {
				b->destroy();
				e->destroy();
				spawnSmallEnemies(e);
				m_score += e->get<CScore>().score;
			}
		}

		for (auto& e : m_entityManager.getEntities("smallEnemy")) {
			if (Physics::CircleCollision(b, e)) {
				b->destroy();
				e->destroy();
				m_score += e->get<CScore>().score;
			}
		}
	}

	// Player Collision
	for (auto& e : m_entityManager.getEntities("enemy")) {
		if (Physics::CircleCollision(m_player, e)) {
			e->destroy();
			spawnPlayer();
			spawnSmallEnemies(e);
		}
	}

	for (auto& e : m_entityManager.getEntities("smallEnemy")) {
		if (Physics::CircleCollision(m_player, e)) {
			e->destroy();
			spawnPlayer();
		}
	}
}

void Scene_Play::sDoAction(const Action& action) {
	if (action.type() == "START") {
		if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
		else if (action.name() == "PAUSE") { setPaused(!m_paused); }
		else if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "UP") { m_player->get<CInput>().up = true; }
		else if (action.name() == "LEFT") { m_player->get<CInput>().left = true; }
		else if (action.name() == "DOWN") { m_player->get<CInput>().down = true; }
		else if (action.name() == "RIGHT") { m_player->get<CInput>().right = true; }
	}
	else if (action.type() == "END") {
		if (action.name() == "UP") { m_player->get<CInput>().up = false; }
		else if (action.name() == "LEFT") { m_player->get<CInput>().left = false; }
		else if (action.name() == "DOWN") { m_player->get<CInput>().down = false; }
		else if (action.name() == "RIGHT") { m_player->get<CInput>().right = false; }
	}
}

void Scene_Play::sEnemySpawner() {
	if (m_paused or !m_isEnemySpawnerActive) { return; }

	auto mEnemyConfig = GameConfig::getInstance().Enemy;
	if (m_currentFrame - m_lastEnemySpawnTime > mEnemyConfig.SpawnInterval) {
		spawnEnemy();
	}
}

void Scene_Play::sMovement() {
	if (m_paused || !m_isMovementActive) { return; }

	{
		auto& cTransform = m_player->get<CTransform>();
		auto& cInput = m_player->get<CInput>();
		auto mPlayerConfig = GameConfig::getInstance().Player;

		// Calculate velocity based on input
		if (cInput.up && !cInput.down) {
			cTransform.velocity.y = -1;
		}
		else if (cInput.down && !cInput.up) {
			cTransform.velocity.y = 1;
		}
		else {
			cTransform.velocity.y = 0;
		}

		if (cInput.left && !cInput.right) {
			cTransform.velocity.x = -1;
		}
		else if (cInput.right && !cInput.left) {
			cTransform.velocity.x = 1;
		}
		else {
			cTransform.velocity.x = 0;
		}

		if (cTransform.velocity) {
			// Normalize so that resultant vector is 1 unit
			cTransform.velocity = cTransform.velocity.normalize();
		}

		// Multiply normalized velocity vector by speed
		cTransform.velocity *= mPlayerConfig.Speed;
	}

	for (auto& e : m_entityManager.getEntities()) {
		if (e->has<CTransform>()) {
			auto& cTransform = e->get<CTransform>();

			// Update position based on velocity
			cTransform.pos += cTransform.velocity;
		}
	}
}

void Scene_Play::sLifespan() {
	if (m_paused || !m_isLifespanActive) { return; }

	for (auto& e : m_entityManager.getEntities()) {
		if (e->has<CLifespan>()) {
			auto& cLifespan = e->get<CLifespan>();
			if (cLifespan.remaining > 0) {
				cLifespan.remaining -= 1;
			}
			else {
				e->destroy();
			}

			if (e->has<CShape>()) {
				auto& cShape = e->get<CShape>();
				auto FC = cShape.circle.getFillColor();
				auto OC = cShape.circle.getOutlineColor();
				auto alpha = (int)(((float)cLifespan.remaining / cLifespan.lifespan) * 255);
				cShape.circle.setFillColor(sf::Color(FC.r, FC.g, FC.b, alpha));
				cShape.circle.setOutlineColor(sf::Color(OC.r, OC.g, OC.b, alpha));
			}
		}

	}
}

void Scene_Play::sRender() {
	if (!m_gameEngine->window().isOpen()) { return; }

	m_gameEngine->window().clear();

	for (auto& e : m_entityManager.getEntities()) {
		auto& cShape = e->get<CShape>();
		auto& cTransform = e->get<CTransform>();

		// set the position of the shape based on the entity's transform->pos
		cShape.circle.setPosition(cTransform.pos);

		// set the rotation of the shape based on the entity's transform->angle
		cTransform.angle += 1.0f;
		cShape.circle.setRotation(sf::degrees(cTransform.angle));

		// draw the entity's sf::CircleShape
		m_gameEngine->window().draw(cShape.circle);
	}

	// draw score
	m_scoreText.setString(std::format("SCORE: {}", m_score));
	m_gameEngine->window().draw(m_scoreText);
}

Vec2f Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity) {
	// TODO: This function takes in a grid (x,y) position and an Entity
	//		 Return a Vec2 indicating where the CENTER position of the Entity should be
	//		 You must use the Entity's Animation size to position it correctly
	//						  entity->get<CAnimation>().getSize()
	//		 The size of the grid width and height is stored in m_gridSize.x and m_gridSize.y
	//		 The bottom-left corner of the Animation should align with the bottom left of the grid cell
	//
	//		 Remember that SFML has (0,0) in the top left, while grid coordinates are specified from bottom-left
	//		 You can get the size of the SFML window via m_game.window().getSize();
	return Vec2f(0, 0);
}

void Scene_Play::spawnPlayer() {
	// check to see if a player already exists before adding a new one
	// if it already exists, just overwrite the values of the existing one
	if (!m_player) {
		m_player = m_entityManager.addEntity("player");
	}

	auto w = m_gameEngine->window().getSize();
	auto mPlayerConfig = GameConfig::getInstance().Player;

	m_player->add<CTransform>(Vec2f((float)w.x / 2, (float)w.y / 2), Vec2f(0.0f, 0.0f), Vec2f(1.f, 1.f), 0.0f);
	m_player->add<CShape>(
		mPlayerConfig.ShapeRadius,
		mPlayerConfig.Vertices,
		sf::Color(
			mPlayerConfig.FillRed,
			mPlayerConfig.FillRed,
			mPlayerConfig.FillBlue),
		sf::Color(
			mPlayerConfig.OutlineRed,
			mPlayerConfig.OutlineGreen,
			mPlayerConfig.OutlineBlue),
		mPlayerConfig.OutlineThickness);
	m_player->add<CBoundingCircle>(mPlayerConfig.CollisionRadius);
	m_player->add<CScore>(0);
	m_player->add<CInput>();
}

// spawn an enemy at a random position
void Scene_Play::spawnEnemy() {
	auto e = m_entityManager.addEntity("enemy");

	auto m = m_gameEngine->window().getSize();
	auto mEnemyConfig = GameConfig::getInstance().Enemy;

	float minSpawnX = mEnemyConfig.CollisionRadius;
	float maxSpawnX = m.x - mEnemyConfig.CollisionRadius;
	float minSpawnY = mEnemyConfig.CollisionRadius;
	float maxSpawnY = m.y - mEnemyConfig.CollisionRadius;

	auto vertices = Random::between(mEnemyConfig.VerticesMin, mEnemyConfig.VerticesMax);

	e->add<CTransform>(
		Vec2f(Random::between(minSpawnX, maxSpawnX), Random::between(minSpawnY, maxSpawnY)),
		Vec2f(
			Random::between(mEnemyConfig.SpeedMin, mEnemyConfig.SpeedMax) * Random::sign(),
			Random::between(mEnemyConfig.SpeedMin, mEnemyConfig.SpeedMax) * Random::sign()),
		Vec2f(1.f, 1.f),
		0.0f);
	e->add<CShape>(
		mEnemyConfig.ShapeRadius,
		vertices,
		sf::Color(Random::between(30, 255), Random::between(30, 255), Random::between(30, 255)),
		sf::Color(mEnemyConfig.OutlineRed, mEnemyConfig.OutlineGreen, mEnemyConfig.OutlineBlue),
		mEnemyConfig.OutlineThickness
	);
	e->add<CBoundingCircle>(mEnemyConfig.CollisionRadius);
	e->add<CScore>(vertices * 100);

	// record when the most recent enemy was spawned
	m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity e) explodes
void Scene_Play::spawnSmallEnemies(std::shared_ptr<Entity> e) {
	auto& cTransform = e->get<CTransform>();
	auto& cScore = e->get<CScore>();
	auto& eCircle = e->get<CShape>().circle;
	float SMALL_SPEED = 5;
	float deltaAngle = (float)360 / eCircle.getPointCount();

	auto mEnemyConfig = GameConfig::getInstance().Enemy;

	for (int i = 0; i < eCircle.getPointCount() + 1; i++) {
		auto s = m_entityManager.addEntity("smallEnemy");

		auto theta = sf::degrees(cTransform.angle + deltaAngle * i);
		auto velocity = Vec2f(SMALL_SPEED * cos(theta.asRadians()), SMALL_SPEED * sin(theta.asRadians()));

		s->add<CTransform>(
			cTransform.pos,
			velocity,
			Vec2f(1.f, 1.f),
			0.0f
		);
		s->add<CShape>(
			mEnemyConfig.ShapeRadius / 2,
			eCircle.getPointCount(),
			eCircle.getFillColor(),
			eCircle.getOutlineColor(),
			eCircle.getOutlineThickness()
		);
		s->add<CBoundingCircle>(mEnemyConfig.CollisionRadius / 2);
		s->add<CScore>(cScore.score * 2);
		s->add<CLifespan>(mEnemyConfig.SmallLifespan);
	}
}

// spawns a bullet from a given entity to a target location
void Scene_Play::spawnBullet(std::shared_ptr<Entity> e, const Vec2f& target) {
	auto& ePosition = e->get<CTransform>().pos;

	auto b = m_entityManager.addEntity("bullet");
	auto mBulletConfig = GameConfig::getInstance().Bullet;

	Vec2f bVelocity = target - ePosition;
	bVelocity = bVelocity.normalize();
	bVelocity *= mBulletConfig.Speed;

	b->add<CTransform>(
		ePosition,
		bVelocity,
		Vec2f(1.f, 1.f),
		0.0f
	);
	b->add<CShape>(
		mBulletConfig.ShapeRadius,
		mBulletConfig.Vertices,
		sf::Color(mBulletConfig.FillRed, mBulletConfig.FillGreen, mBulletConfig.FillBlue),
		sf::Color(mBulletConfig.OutlineRed, mBulletConfig.OutlineGreen, mBulletConfig.OutlineBlue),
		mBulletConfig.OutlineThickness
	);
	b->add<CBoundingCircle>(mBulletConfig.CollisionRadius);
	b->add<CLifespan>(mBulletConfig.Lifespan);
}

void Scene_Play::spawnSpecialWeapon(std::shared_ptr<Entity> e) {
	// TODO: implement your own speacial weapon
}
