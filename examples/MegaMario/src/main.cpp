#include "GameEngine.hpp"
#include "MegaMario_Menu.hpp"
#include "MegaMario_Play.hpp"

#define MEGA_MARIO_IMPL

int main() {
	auto engine = std::make_shared<GameEngine>();
	engine->changeScene("MegaMario_Menu", std::make_shared<MegaMario_Menu>(engine));
	engine->run();

	return 0;
}
