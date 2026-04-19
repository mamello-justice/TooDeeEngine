#include "Entity.hpp"

#include <string>
#include <tuple>

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
#include <quickjs.h>
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
JSValue Entity::operator()(JSContext* ctx) const {
	JSValue jsE = JS_NewObject(ctx);

	JSValue jsComps = JS_NewObject(ctx);
	JS_SetPropertyStr(ctx, jsE, "components", JS_DupValue(ctx, jsComps));

	if (has<CTransform>()) {
		JSValue jsComp = get<CTransform>()(ctx);
		JS_SetPropertyStr(ctx, jsComps, "transform", JS_DupValue(ctx, jsComp));
		JS_FreeValue(ctx, jsComp);
	}

	if (has<CBoundingBox>()) {
		JSValue jsComp = get<CBoundingBox>()(ctx);
		JS_SetPropertyStr(ctx, jsComps, "boundingBox", JS_DupValue(ctx, jsComp));
		JS_FreeValue(ctx, jsComp);
	}

	if (has<CBoundingCircle>()) {
		JSValue jsComp = get<CBoundingCircle>()(ctx);
		JS_SetPropertyStr(ctx, jsComps, "boundingCircle", JS_DupValue(ctx, jsComp));
		JS_FreeValue(ctx, jsComp);
	}

	JS_FreeValue(ctx, jsComps);

	return jsE;
}
#endif
