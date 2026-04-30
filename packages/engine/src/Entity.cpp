#include "Entity.hpp"

#include <string>
#include <tuple>

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
#include <quickjspp.h>
#endif

#include "Components.hpp"

Entity::Entity(const size_t& id, const std::string& tag) : mId(id), mTag(tag) {}

size_t Entity::id() const {
	return mId;
}

bool Entity::isAlive() const {
	return mAlive;
}

void Entity::destroy() {
	// TODO: Run onDestroy script function
	mAlive = false;
}

const std::string& Entity::tag() const {
	return mTag;
}

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
void Entity::operator()(JSContext* ctx, const JSValue& entity) {
	auto jsEntity = qjs::js_traits<Entity>::unwrap(ctx, entity);

	if (jsEntity.has<CTransform>()) {
		auto& cTrans = get<CTransform>();
		auto jsTrans = jsEntity.get<CTransform>();

		cTrans.velocity = jsTrans.velocity;
	}
}
#endif
