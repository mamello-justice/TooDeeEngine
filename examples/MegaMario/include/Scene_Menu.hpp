#pragma once

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

#include "Action.hpp"
#include "GameEngine.hpp"
#include "Scene.hpp"

class Scene_Menu : public Scene
{
	std::vector<std::string> m_levelPaths;
	std::vector<std::string> m_menuStrings;
	size_t m_selectedMenuIndex = 0;

protected:
	void init();
	void update() override;

	void onEnd();

	void sDoAction(const Action& action) override;
	void sRender() override;

	void registerLevel(const std::string& name, const std::string& path);

public:
	Scene_Menu(std::shared_ptr<GameEngine> gameEngine);
};
