#include "GameEngine.hpp"
#include "Scene_Menu.hpp"

int main()
{
	auto engine = std::make_shared<GameEngine>();
	engine->changeScene("Scene_Menu", std::make_shared<Scene_Menu>(engine));
	engine->run();

	return 0;
}
