#include "Scene_Play.hpp"

#include <SFML/Graphics.hpp>

#include "Action.hpp"
#include "Assets.hpp"
#include "Components.hpp"
#include "GameEngine.hpp"
#include "Physics.hpp"
#include "Scene_Menu.hpp"

Scene_Play::Scene_Play(std::shared_ptr<GameEngine> gameEngine, const std::string& levelPath)
	:
	Scene(gameEngine),
	m_levelPath(levelPath),
	m_gridText(Assets::Instance().getFont("Tech"), "", 10) {
	init(m_levelPath);
}

void Scene_Play::init(const std::string& levelPath) {
	registerAction(sf::Keyboard::Scancode::P, "PAUSE");
	registerAction(sf::Keyboard::Scancode::Escape, "QUIT");
	registerAction(sf::Keyboard::Scancode::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::Scancode::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::Scancode::G, "TOGGLE_GRID");
	registerAction(sf::Keyboard::Scancode::N, "TOGGLE_ANIM_NAME");
	registerAction(sf::Keyboard::Scancode::Space, "UP");
	registerAction(sf::Keyboard::Scancode::W, "UP");
	registerAction(sf::Keyboard::Scancode::A, "LEFT");
	registerAction(sf::Keyboard::Scancode::S, "DOWN");
	registerAction(sf::Keyboard::Scancode::D, "RIGHT");

	// TODO: Register all other gameplay Actions

	loadLevel(levelPath);

	setPaused(false);
}

void Scene_Play::update() {
	m_entityManager.update();

	// TODO: implement pause functionality

	sMovement();
	sLifespan();
	sCollision();
	sAnimation();
	sRender();
}

void Scene_Play::loadLevel(const std::string& filename) {
	// reset the entity manager every time we load a level
	m_entityManager = EntityManager();

	std::ifstream file(filename);
	std::string str;

	while (file.good()) {
		file >> str;
		if (str == "Tile") {
			std::string animName;
			Vec2f gridPos;
			file >> animName >> gridPos.x >> gridPos.y;
			addTile(animName, gridPos);
		}
		else if (str == "Dec") {
			std::string animName;
			Vec2f gridPos;
			file >> animName >> gridPos.x >> gridPos.y;
			addDecoration(animName, gridPos);
		}
		else if (str == "Player") {
			file >>
				m_playerConfig.X >>
				m_playerConfig.Y >>
				m_playerConfig.CW >>
				m_playerConfig.CH >>
				m_playerConfig.SX >>
				m_playerConfig.SY >>
				m_playerConfig.SM >>
				m_playerConfig.GY >>
				m_playerConfig.B;
			spawnPlayer();
		}
	}
}

void Scene_Play::onEnd() {
	m_gameEngine->changeScene("Scene_Menu", std::make_shared<Scene_Menu>(m_gameEngine));
}

void Scene_Play::sAnimation() {
	for (auto e : m_entityManager.getEntities()) {
		if (e->has<CAnimation>()) {
			auto& cAnim = e->get<CAnimation>();
			cAnim.animation.update();

			if (!cAnim.repeat && cAnim.animation.hasEnded()) {
				e->destroy();
			}
		}
	}

	if (m_player->get<CState>().state == "stand")
	{
		if (m_player->get<CAnimation>().animation.getName() != "Stand") {
			m_player->add<CAnimation>(Assets::Instance().getAnimation("Stand"), true);
		}
	}
	else if (m_player->get<CState>().state == "run")
	{
		if (m_player->get<CAnimation>().animation.getName() != "Run") {
			m_player->add<CAnimation>(Assets::Instance().getAnimation("Run"), true);
		}
	}
	else if (m_player->get<CState>().state == "air")
	{
		if (m_player->get<CAnimation>().animation.getName() != "Air") {
			m_player->add<CAnimation>(Assets::Instance().getAnimation("Air"), true);
		}
	}
}

void Scene_Play::sCollision() {
	// REMEMBER: SFML's (0,0) position is on the TOP-LEFT corner
	//			 This means jumping will have a negative y�component
	//			 and gravity will have a positive y - component
	//			 Also, something BELOW something else will have a y value GREATER than it
	//			 Also, something ABOVE something else will have a y value LESS than it

	// TODO: Implement Physics::GetOverlap() function, use it inside this function

	// TODO: Implement bullet / tile collisions
	//		 Destroy the tile if it has a Brick animation
	// TODO: Implement player / tile collisions and resolutions
	//	 	 Update the CState component of the player to store whether
	//		 it is currently on the ground or in the air. This will be
	//		 used by the Animation system
	// TODO: Check to see if the player has fallen down a hole (y > height())
	// TODO: Don't let the play er walk off the side of the map

	// Player Overlap

	bool inAir = true;

	for (auto& tile : m_entityManager.getEntities("tile")) {
		if (tile->has<CBoundingBox>()) {
			auto overlap = Physics::GetOverlap(m_player, tile);


			if (overlap.x > 0 && overlap.y > 0) {
				m_player->get<CTransform>().pos.y -= overlap.y;
				m_player->get<CInput>().canJump = true;
				inAir = false;
			}

		}
	}


	if (inAir) {
		m_player->get<CState>().state = "air";
	}
	else {
		m_player->get<CState>().state = "stand";
	}
}

void Scene_Play::sDoAction(const Action& action) {
	if (action.type() == "START")
	{
		if (action.name() == "TOGGLE_TEXTURE")
		{
			m_drawTextures = !m_drawTextures;
		}
		else if (action.name() == "TOGGLE_COLLISION")
		{
			m_drawCollision = !m_drawCollision;
		}
		else if (action.name() == "TOGGLE_GRID")
		{
			m_drawGrid = !m_drawGrid;
		}
		else if (action.name() == "TOGGLE_ANIM_NAME")
		{
			m_drawAnimName = !m_drawAnimName;
		}
		else if (action.name() == "PAUSE")
		{
			setPaused(!m_paused);
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
		else if (action.name() == "UP")
		{
			m_player->get<CInput>().up = true;
		}
		else if (action.name() == "LEFT")
		{
			m_player->get<CInput>().left = true;
		}
		else if (action.name() == "DOWN")
		{
			m_player->get<CInput>().down = true;
		}
		else if (action.name() == "RIGHT")
		{
			m_player->get<CInput>().right = true;
		}
	}
	else if (action.type() == "END")
	{
		if (action.name() == "UP")
		{
			m_player->get<CInput>().up = false;
		}
		else if (action.name() == "LEFT")
		{
			m_player->get<CInput>().left = false;
		}
		else if (action.name() == "DOWN")
		{
			m_player->get<CInput>().down = false;
		}
		else if (action.name() == "RIGHT")
		{
			m_player->get<CInput>().right = false;
		}
	}
}

void Scene_Play::sEnemySpawner() {}

void Scene_Play::sLifespan() {
	// TODO: Check lifespan of entities that have them, and destroy them if they go over
}

void Scene_Play::sMovement() {
	// Gravity component
	for (auto& e : m_entityManager.getEntities()) {
		if (e->has<CGravity>()) {
			e->get<CTransform>().velocity.y += e->get<CGravity>().gravity;
		}
	}

	// Player movement
	if (m_player->get<CInput>().right && !m_player->get<CInput>().left) {
		m_player->get<CTransform>().velocity.x = m_playerConfig.SX;
	}
	else if (m_player->get<CInput>().left && !m_player->get<CInput>().right) {
		m_player->get<CTransform>().velocity.x = -m_playerConfig.SX;
	}
	else {
		m_player->get<CTransform>().velocity.x = 0;
	}
	if (m_player->get<CInput>().up && m_player->get<CInput>().canJump) {
		m_player->get<CTransform>().velocity.y = m_playerConfig.SY;
		m_player->get<CInput>().canJump = false;
	}
	if (!m_player->get<CInput>().up && m_player->get<CTransform>().velocity.y < 0) {
		m_player->get<CTransform>().velocity.y = 0;
	}

	// Cap velocity to max speed
	m_player->get<CTransform>().velocity.y = std::min(m_player->get<CTransform>().velocity.y, m_playerConfig.SM);
	m_player->get<CTransform>().velocity.x = std::min(m_player->get<CTransform>().velocity.x, m_playerConfig.SM);

	for (auto& e : m_entityManager.getEntities()) {
		if (e->has<CTransform>()) {
			e->get<CTransform>().pos += e->get<CTransform>().velocity;
		}
	}

	if (m_player->get<CTransform>().velocity.x) {
		m_player->get<CState>().state = "run";
	}
}

void Scene_Play::sRender() {
	// color the background darker so you know that the game is paused
	m_gameEngine->window().clear(m_paused ? sf::Color(50, 50, 150) : sf::Color(97, 133, 248));

	auto wWidth = m_gameEngine->window().getSize().x;
	auto wHeight = m_gameEngine->window().getSize().y;

	// set the viewport of the window to be centered on the player if it's far enough right
	auto& pos = m_player->get<CTransform>().pos;
	float windowCenterX = std::max((float)wWidth / 2.0f, pos.x);
	sf::View view = m_gameEngine->window().getView();
	view.setCenter({ windowCenterX, (float)wHeight - view.getCenter().y });
	m_gameEngine->window().setView(view);

	// draw all Entity textures / animations
	if (m_drawTextures) {
		for (const auto& e : m_entityManager.getEntities()) {
			if (e->has<CAnimation>() && e->has<CTransform>()) {
				auto& cTrans = e->get<CTransform>();
				auto& anim = e->get<CAnimation>().animation;
				anim.getSprite()->setRotation(sf::radians(cTrans.angle));
				anim.getSprite()->setPosition({ cTrans.pos.x, cTrans.pos.y });
				anim.getSprite()->setScale({ cTrans.scale.x, cTrans.scale.y });
				m_gameEngine->window().draw(*anim.getSprite());
			}
		}
	}

	// draw all Entity collision bounding boxes with a rectangle shape
	if (m_drawCollision) {
		for (const auto& e : m_entityManager.getEntities()) {
			if (e->has<CBoundingBox>() && e->has<CTransform>()) {
				auto& box = e->get<CBoundingBox>();
				auto& cTrans = e->get<CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rect.setPosition({ cTrans.pos.x, cTrans.pos.y });
				rect.setFillColor(sf::Color(0, 0, 0, 0));
				rect.setOutlineColor(sf::Color::White);
				rect.setOutlineThickness(1);
				m_gameEngine->window().draw(rect);
			}
		}
	}

	if (m_drawAnimName) {
		for (const auto& e : m_entityManager.getEntities()) {
			if (e->has<CAnimation>() && e->has<CTransform>()) {
				auto& cTrans = e->get<CTransform>();
				auto& anim = e->get<CAnimation>().animation;
				sf::Text name(Assets::Instance().getFont("Tech"), anim.getName());
				name.setPosition({ cTrans.pos.x, cTrans.pos.y });
				m_gameEngine->window().draw(name);
			}
		}
	}

	// draw the grid so that can easily debug
	if (m_drawGrid) {
		float leftX = float(m_gameEngine->window().getView().getCenter().x) - wWidth / 2.0f;
		float rightX = leftX + wWidth + m_gridSize.x;
		float nextGridX = leftX - float((int)leftX % (int)m_gridSize.x);

		for (float x = nextGridX; x < rightX; x += float(m_gridSize.x)) {
			sf::Vertex line[] = {
					sf::Vertex{Vec2f(x, 0)},
					sf::Vertex{Vec2f(x, wHeight)}
			};

			m_gameEngine->window().draw(line, 2, sf::PrimitiveType::Lines);
		}

		for (float y = 0; y < wHeight; y += float(m_gridSize.y)) {
			sf::Vertex line[] = {
					sf::Vertex{Vec2f(leftX, wHeight - y)},
					sf::Vertex{Vec2f(rightX, wHeight - y)}
			};

			m_gameEngine->window().draw(line, 2, sf::PrimitiveType::Lines);

			for (float x = nextGridX; x < rightX; x += float(m_gridSize.x)) {
				std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
				std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
				m_gridText.setString("(" + xCell + "," + yCell + ")");
				m_gridText.setPosition({ x + 3, wHeight - y - m_gridSize.y + 2 });
				m_gameEngine->window().draw(m_gridText);
			}
		}
	}
}

Vec2f Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity) {
	auto size = entity->get<CAnimation>().animation.getRect().size;
	return Vec2f(
		gridX * m_gridSize.x + size.x,
		m_gameEngine->window().getSize().y - (gridY * m_gridSize.y + size.y));
}

void Scene_Play::spawnPlayer() {
	// check to see if a player already exists before adding a new one
	// if it already exists, just overwrite the values of the existing one
	if (!m_player)
	{
		m_player = m_entityManager.addEntity("player");
	}

	// here is a sample player entity which you can use to construct other entities
	m_player->add<CAnimation>(Assets::Instance().getAnimation("Stand"), true);
	m_player->add<CTransform>(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
	m_player->add<CBoundingBox>(Vec2f(m_playerConfig.CW, m_playerConfig.CH));
	m_player->add<CState>("stand");
	m_player->add<CGravity>(m_playerConfig.GY);
	m_player->add<CInput>();


	// TODO: be sure to add the remaining components to the player
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity) {
	// TODO: this should spawn a bullet at the given entity,
	//		 going in the direction the entity is facing
}

void Scene_Play::addTile(const std::string& animName, const Vec2f& gridPos) {
	auto tile = m_entityManager.addEntity("tile");

	std::cout << "Adding tile @ (" << gridPos.x << ", " << gridPos.y << ") with Animation: " << animName << std::endl;
	auto& anim = Assets::Instance().getAnimation(animName);
	tile->add<CAnimation>(anim, true);
	tile->add<CTransform>(gridToMidPixel(gridPos.x, gridPos.y, tile));
	tile->add<CBoundingBox>(sf::Vector2f(anim.getRect().size));
}

void Scene_Play::addDecoration(const std::string& animName, const Vec2f& gridPos) {

}

void Scene_Play::onFrame() {}
