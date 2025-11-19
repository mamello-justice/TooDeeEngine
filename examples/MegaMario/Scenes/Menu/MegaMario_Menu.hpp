#pragma once

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

#include "Action.hpp"
#include "GameEngine.hpp"
#include "Scene.hpp"

class MegaMario_Menu : public Scene
{
	std::vector<std::function<void()>> m_systems;
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
	MegaMario_Menu(std::shared_ptr<GameEngine> gameEngine);
};
