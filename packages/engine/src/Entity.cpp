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
namespace qjs
{
	Entity js_traits<Entity>::unwrap(JSContext* ctx, JSValueConst val) {
		Entity result;

		JSAtom compName = JS_NewAtom(ctx, "components");
		int hasComponents = JS_HasProperty(ctx, val, compName);
		JS_FreeAtom(ctx, compName);

		if (hasComponents) {
			JSValue components = JS_GetPropertyStr(ctx, val, "components");
		}
		else {
			throw std::runtime_error("Does not have property");
		}

		return result;
	}

	JSValue js_traits<Entity>::wrap(JSContext* ctx, const Entity& e) noexcept {
		JSValue result = JS_NewObject(ctx);

		JSValue components = JS_NewObject(ctx);

		if (e.has<CTransform>())
			JS_SetPropertyStr(ctx, components, "transform", js_traits<CTransform>::wrap(ctx, e.get<CTransform>()));
		if (e.has<CBoundingBox>())
			JS_SetPropertyStr(ctx, components, "boundingBox", js_traits<CBoundingBox>::wrap(ctx, e.get<CBoundingBox>()));
		if (e.has<CBoundingCircle>())
			JS_SetPropertyStr(ctx, components, "boundingCircle", js_traits<CBoundingCircle>::wrap(ctx, e.get<CBoundingCircle>()));

		JS_SetPropertyStr(ctx, result, "components", components);

		JS_FreeValue(ctx, components);

		return result;
	}
} // namespace qjs
#endif
