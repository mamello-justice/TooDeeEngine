#include "GameEngine.hpp"

#include <string>
#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>

#include "spdlog/spdlog.h"

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
#include "quickjs.h"
#include "qjs.hpp"
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

GameEngine::GameEngine(bool rendering) :
	m_shouldRender(rendering) {
	init();
}

GameEngine::~GameEngine() {
	if (m_jsContext) {
		JS_FreeContext(m_jsContext);
		m_jsContext = nullptr;
	}
	if (m_jsRuntime) {
		JS_FreeRuntime(m_jsRuntime);
		m_jsRuntime = nullptr;
	}
}

void GameEngine::init() {
#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
	m_jsRuntime = JS_NewRuntime();

	if (m_jsRuntime) {
		m_jsContext = JS_NewContext(m_jsRuntime);
		if (!m_jsContext) {
			JS_FreeRuntime(m_jsRuntime);
			m_jsRuntime = nullptr;
		}
	}
#endif

	m_preSceneSystems.push_back(std::bind(&GameEngine::sUserInput, this));
	m_preSceneSystems.push_back(std::bind(&GameEngine::sMovement, this));
	m_preSceneSystems.push_back(std::bind(&GameEngine::sScripting, this));
	m_preSceneSystems.push_back(std::bind(&GameEngine::sCollision, this));
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

const sf::RenderTexture& GameEngine::renderTarget() const {
	return m_renderTarget;
}

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
	// Setup Global
	JSValue jsGlobal = JS_GetGlobalObject(m_jsContext);
	JSValue jsEngine = JS_GetValueOf(m_jsContext, *this);
	JS_SetPropertyStr(m_jsContext, jsGlobal, "TooDeeEngine", jsEngine);

	// Setup Script Module
	auto scriptName = e->get<CQJSScript>().name;
	auto& script = Assets::Instance().getScript(scriptName);
	JSValue moduleEval = JS_Eval(m_jsContext, script.getContent().c_str(), script.getContent().size(), script.getPath().c_str(), JS_EVAL_TYPE_MODULE);

	if (JS_IsException(moduleEval)) {
		JSValue exception = JS_GetException(m_jsContext);
		const char* str = JS_ToCString(m_jsContext, exception);
		std::cerr << "Exception in script " << scriptName << ": " << str << std::endl;
		JS_FreeCString(m_jsContext, str);
		JS_FreeValue(m_jsContext, exception);
	}

	JS_FreeValue(m_jsContext, moduleEval);

	std::string mainScript =
		std::format("import * as script from '{}';\n", script.getPath()) +
		"globalThis.onUpdate = script.onUpdate;";

	JSValue mainEval = JS_Eval(m_jsContext, mainScript.c_str(), mainScript.size(), "main.js", JS_EVAL_TYPE_MODULE);

	if (JS_IsException(mainEval)) {
		JSValue exception = JS_GetException(m_jsContext);
		const char* str = JS_ToCString(m_jsContext, exception);
		std::cerr << "Exception in script main.js: " << str << std::endl;
		JS_FreeCString(m_jsContext, str);
		JS_FreeValue(m_jsContext, exception);
	}

	JS_FreeValue(m_jsContext, mainEval);

	// Entity -> JSValue
	JSValue jsEntity = JS_GetValueOf(m_jsContext, *e);

	// Execute Update Script
	JSValue jsOnUpdate = JS_GetPropertyStr(m_jsContext, jsGlobal, "onUpdate");

	JSValue args[1] = { jsEntity };

	JSValue result = JS_Call(m_jsContext, jsOnUpdate, jsGlobal, 2, args);
	if (JS_IsException(result)) {
		JSValue exception = JS_GetException(m_jsContext);
		const char* str = JS_ToCString(m_jsContext, exception);
		std::cerr << "Failed to call onUpdate: " << scriptName << "\n\t" << str << std::endl;
		JS_FreeCString(m_jsContext, str);
		JS_FreeValue(m_jsContext, exception);
	}

	JS_UpdateFromValue(m_jsContext, jsEntity, *e);
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
