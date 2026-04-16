#include "GameEngine.hpp"

#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
#include <quickjs.h>
#endif

#include "Assets.hpp"
#include "Components.hpp"
#include "Physics.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Vec2.hpp"

GameEngine::GameEngine() {
	init();
}

GameEngine::GameEngine(bool rendering) : m_shouldRender(rendering) {
	init();
}

GameEngine::~GameEngine() {
#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
	if (m_jsRuntime) {
		JS_FreeRuntime(m_jsRuntime);
		m_jsRuntime = nullptr;
	}
	if (m_jsContext) {
		JS_FreeContext(m_jsContext);
		m_jsContext = nullptr;
	}
#endif

	m_window.close();
}

void GameEngine::init() {
	m_preSceneSystems.push_back(std::bind(&GameEngine::sUserInput, this));
	m_preSceneSystems.push_back(std::bind(&GameEngine::sMovement, this));
	m_preSceneSystems.push_back(std::bind(&GameEngine::sScripting, this));
	m_preSceneSystems.push_back(std::bind(&GameEngine::sCollision, this));

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
	// Create runtime
	m_jsRuntime = JS_NewRuntime();
	if (!m_jsRuntime) {
		throw std::runtime_error("Failed to create JS runtime");
	}
	m_jsContext = JS_NewContext(m_jsRuntime);
	if (!m_jsContext) {
		JS_FreeRuntime(m_jsRuntime);
		throw std::runtime_error("Failed to create JS runtime");
	}
#endif
}

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
void GameEngine::setupQJSDebug() {
	// Enable various debug output flags (combine them with |)
	uint32_t dump_flags = 0;

	dump_flags |= JS_DUMP_FREE;        // 0x200  - dump every object free
	dump_flags |= JS_DUMP_GC;          // 0x400  - dump GC occurrences
	dump_flags |= JS_DUMP_GC_FREE;     // 0x800  - dump objects freed by GC
	dump_flags |= JS_DUMP_LEAKS;       // 0x4000 - dump leaked objects and strings
	dump_flags |= JS_DUMP_ATOM_LEAKS;  // 0x8000 - dump leaked atoms
	dump_flags |= JS_DUMP_MEM;         // 0x10000 - dump memory usage at runtime
	dump_flags |= JS_DUMP_OBJECTS;     // 0x20000 - dump objects in JS_FreeRuntime

	// JS_SetDumpFlags(m_jsRuntime, dump_flags);
}
#endif

void GameEngine::update() {
	if (!m_running) { return; }

	if (m_sceneMap.empty()) { return; }

	for (auto system : m_preSceneSystems) { system(); }

	if (currentScene()) {
		currentScene()->getEntityManager().update();
		currentScene()->update();
	}

	for (auto system : m_postSceneSystems) { system(); }
}

void GameEngine::quit() { m_running = false; }

sf::RenderTexture& GameEngine::renderTarget() {
	return m_renderTarget;
}

sf::RenderWindow& GameEngine::window() {
	return m_window;
}

std::shared_ptr<Scene> GameEngine::currentScene() {
	return hasScene(m_currentScene) ? m_sceneMap.at(m_currentScene) : nullptr;
}

bool GameEngine::getIsRunning() const {
	return m_running;
}

bool GameEngine::hasScenes() const {
	return m_sceneMap.empty();
}

bool GameEngine::hasScene(const std::string& name) const {
	return m_sceneMap.find(name) != m_sceneMap.end();
}

void GameEngine::removeScene(std::shared_ptr<Scene> scene) {
	std::erase_if(m_sceneMap, [scene](const auto& pair) {
		return pair.second == scene;
		});
}

void GameEngine::removeScene(const std::string& name) {
	m_sceneMap.erase(name);
}

void GameEngine::handleEvent(std::optional<sf::Event> event) {
	if (event->is<sf::Event::Closed>()) { quit(); }

	// this event is triggered when a key is pressed
	if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
		auto it = currentScene()->getActionMap().find(keyPressed->scancode);
		if (it == currentScene()->getActionMap().end()) { return; }

		Action action(it->second, "START");
		currentScene()->sDoAction(action);
	}

	// this event is triggered when a key is released
	if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
		auto it = currentScene()->getActionMap().find(keyReleased->scancode);
		if (it == currentScene()->getActionMap().end()) { return; }

		Action action(it->second, "END");
		currentScene()->sDoAction(action);
	}

	// this event is triggered when we click on window with mouse
	if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
		Vec2f mPos(mousePressed->position);
		currentScene()->sClickHandler(mPos, mousePressed->button);
	}


	if (const auto* mousePressed = event->getIf<sf::Event::MouseMoved>()) {
		Vec2f mPos(mousePressed->position);
		currentScene()->sHoverHandler(mPos);
	}
}

void GameEngine::sUserInput() {
	while (auto event = m_window.pollEvent()) {
		handleEvent(event);
	}
}

void GameEngine::sMovement() {
	if (currentScene()) {
		if (currentScene()->isPaused()) { return; }

		for (const auto& e : currentScene()->getEntityManager().getEntities()) {
			if (e->has<CTransform>()) {
				auto& c = e->get<CTransform>();
				c.pos += c.velocity;
			}
		}
	}
}

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
void GameEngine::handleJavascriptScriptExecution(std::shared_ptr<Entity> e) {
	JSValue jsGlobal = JS_GetGlobalObject(m_jsContext);

	// Create TooDeeEngine object in JS context and set as global variable
	JSValue jsEngine = (*this)(m_jsContext);
	JS_SetPropertyStr(m_jsContext, jsGlobal, "TooDeeEngine", JS_DupValue(m_jsContext, jsEngine));
	JS_FreeValue(m_jsContext, jsEngine);

	auto& cScript = e->get<CQJSScript>();
	auto& script = Assets::Instance().getScript(cScript.name);
	JSValue scriptEval = JS_Eval(m_jsContext, script.getContent().c_str(), script.getContent().size(), script.getPath().c_str(), JS_EVAL_TYPE_GLOBAL);

	if (JS_IsException(scriptEval)) {
		JSValue exception = JS_GetException(m_jsContext);
		const char* str = JS_ToCString(m_jsContext, exception);
		std::cerr << "Exception in script " << cScript.name << ": " << str << std::endl;
		JS_FreeCString(m_jsContext, str);
		JS_FreeValue(m_jsContext, exception);
	}

	JS_FreeValue(m_jsContext, scriptEval);

	JSValue jsUpdateFunc = JS_GetPropertyStr(m_jsContext, jsGlobal, "onUpdate");

	JSValue jsEntity = (*e)(m_jsContext);

	JSValue args[1];
	args[0] = jsEntity;

	JSValue updateCall = JS_Call(m_jsContext, jsUpdateFunc, jsGlobal, 1, args);


	if (JS_IsException(updateCall)) {
		JSValue exception = JS_GetException(m_jsContext);
		const char* str = JS_ToCString(m_jsContext, exception);
		std::cerr << "Exception in script " << cScript.name << ": " << str << std::endl;
		JS_FreeCString(m_jsContext, str);
		JS_FreeValue(m_jsContext, exception);
	}
	else {
		JSValue jsComps = JS_GetPropertyStr(m_jsContext, jsEntity, "components");

		if (e->has<CTransform>()) {
			auto& cTrans = e->get<CTransform>();
			JSValue jsTrans = JS_GetPropertyStr(m_jsContext, jsComps, "transform");

			JSValue jsVel = JS_GetPropertyStr(m_jsContext, jsTrans, "velocity");
			JSValue jsX = JS_GetPropertyStr(m_jsContext, jsVel, "x");
			JSValue jsY = JS_GetPropertyStr(m_jsContext, jsVel, "y");

			double x, y;

			if (JS_ToFloat64(m_jsContext, &x, jsX)) {}
			if (JS_ToFloat64(m_jsContext, &y, jsY)) {}

			cTrans.velocity.x = x;
			cTrans.velocity.y = y;

			JS_FreeValue(m_jsContext, jsX);
			JS_FreeValue(m_jsContext, jsY);
			JS_FreeValue(m_jsContext, jsVel);
			JS_FreeValue(m_jsContext, jsTrans);
		}

		JS_FreeValue(m_jsContext, jsComps);
	}

	JS_FreeValue(m_jsContext, jsEntity);
	JS_FreeValue(m_jsContext, updateCall);
	JS_FreeValue(m_jsContext, jsUpdateFunc);
}
#endif

void GameEngine::sScripting() {
	if (currentScene()) {
		if (currentScene()->isPaused()) { return; }

		for (const auto& e : currentScene()->getEntityManager().getEntities()) {
			if (e->has<CNativeScript>()) {
				auto& c = e->get<CNativeScript>();
				c.onUpdate(*e);
			}

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
			if (e->has<CQJSScript>()) {
				handleJavascriptScriptExecution(e);
			}
#endif
		}
	}
}

void GameEngine::sCollision() {
	if (currentScene()) {
		if (currentScene()->isPaused()) { return; }

		for (const auto& e : currentScene()->getEntityManager().getEntities()) {
			if (!e->has<CBoundingBox>() && !e->has<CBoundingCircle>()) { continue; }

			for (const auto& e2 : currentScene()->getEntityManager().getEntities()) {
				if (e == e2) { continue; }
				if (!e2->has<CBoundingBox>() && !e2->has<CBoundingCircle>()) { continue; }

				Vec2f overlap = Physics::GetOverlap(e, e2);
				if (overlap.x > 0 && overlap.y > 0) {
					if (e->has<CTransform>()) {
						auto& cTrans = e->get<CTransform>();
					}

					if (e2->has<CTransform>()) {
						auto& cTrans = e->get<CTransform>();
					}
				}

			}

		}
	}

}

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
JSValue GameEngine::operator()(JSContext* ctx) const {
	JSValue jsGE = JS_NewObject(ctx);

	JSValue jsRT = JS_NewObject(ctx);
	JS_SetPropertyStr(ctx, jsGE, "renderTarget", JS_DupValue(ctx, jsRT));

	Vec2u size = m_renderTarget.getSize();
	JSValue jsSize = size(ctx);
	JS_SetPropertyStr(ctx, jsRT, "size", JS_DupValue(ctx, jsSize));
	JS_FreeValue(ctx, jsSize);

	JS_FreeValue(ctx, jsRT);

	return jsGE;
}
#endif
