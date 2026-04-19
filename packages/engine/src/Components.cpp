#include "Components.hpp"

#include <SFML/Graphics.hpp>

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
#include <quickjs.h>
#endif

#include "Animation.hpp"
#include "Entity.hpp"
#include "Vec2.hpp"

CLabel::CLabel() : label("") {}

CLabel::CLabel(const std::string& l) : label(l) {}

CTransform::CTransform(const Vec2f& p) : pos(p), prevPos(p) {}

CTransform::CTransform(const Vec2f& p, const Vec2f& v) : pos(p), prevPos(p), velocity(v) {}

CTransform::CTransform(const Vec2f& p, const Vec2f& v, const Vec2f& s, float a)
	: pos(p), prevPos(p), velocity(v), scale(s), angle(a) {}

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
JSValue CTransform::operator()(JSContext* ctx) const {
	JSValue jsTrans = JS_NewObject(ctx);

	JSValue jsPos = pos(ctx);
	JS_SetPropertyStr(ctx, jsTrans, "pos", JS_DupValue(ctx, jsPos));
	JS_FreeValue(ctx, jsPos);

	JSValue jsVel = velocity(ctx);
	JS_SetPropertyStr(ctx, jsTrans, "velocity", JS_DupValue(ctx, jsVel));
	JS_FreeValue(ctx, jsVel);

	return jsTrans;
}
#endif

CCircle::CCircle(float radius, size_t points, const sf::Color& fill, const sf::Color& outline, float thickness) :
	points(points), thickness(thickness), radius(radius), fill(fill), outline(outline) {}

CRectangle::CRectangle(const Vec2f& size, const sf::Color& fill, const sf::Color& outline, float thickness) :
	thickness(thickness), size(size), fill(fill), outline(outline) {}

CScore::CScore(int s) : score(s) {}

CLifespan::CLifespan(int totalLifespan)
	: lifespan(totalLifespan), remaining(totalLifespan) {}

CBoundingBox::CBoundingBox(const Vec2f& s) :
	size(s), halfSize(s.x / 2, s.y / 2) {}

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
JSValue CBoundingBox::operator()(JSContext* ctx) const {
	JSValue jsTrans = JS_NewObject(ctx);

	JSValue jsSize = size(ctx);
	JS_SetPropertyStr(ctx, jsTrans, "size", JS_DupValue(ctx, jsSize));
	JS_FreeValue(ctx, jsSize);

	JSValue jsHalfSize = halfSize(ctx);
	JS_SetPropertyStr(ctx, jsTrans, "halfSize", JS_DupValue(ctx, jsHalfSize));
	JS_FreeValue(ctx, jsHalfSize);

	return jsTrans;
}
#endif

CBoundingCircle::CBoundingCircle(float r) : radius(r) {}

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
JSValue CBoundingCircle::operator()(JSContext* ctx) const {
	JSValue jsTrans = JS_NewObject(ctx);

	JSValue jsRadius = JS_NewFloat64(ctx, radius);
	JS_SetPropertyStr(ctx, jsTrans, "radius", JS_DupValue(ctx, jsRadius));
	JS_FreeValue(ctx, jsRadius);

	return jsTrans;
}
#endif

CAnimation::CAnimation(const Animation& anim, bool r) :
	animation(anim), repeat(r) {}

CGravity::CGravity(float g) : gravity(g) {}

CState::CState(const std::string& s) : state(s) {}

CNativeScript::CNativeScript(const std::function<void(Entity&)>& updateFunc) : onUpdate(updateFunc) {
	// TODO: Run onCreate script function
}

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
CQJSScript::CQJSScript(const std::string& n) : name(n) {
	// TODO: Run onCreate script function
}
#endif

std::string getComponentName(ComponentEnum c) {
	switch (c) {
	case ComponentEnum::Animation:
		return "Animation";
	case ComponentEnum::BoundingBox:
		return "Box Collider";
	case ComponentEnum::BoundingCircle:
		return "Circle Collider";
	case ComponentEnum::Circle:
		return "Circle";
	case ComponentEnum::Gravity:
		return "Gravity";
	case ComponentEnum::Input:
		return "Input";
#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
	case ComponentEnum::QJSScript:
		return "QJS Script";
#endif
	case ComponentEnum::Label:
		return "Label";
	case ComponentEnum::Lifespan:
		return "Lifespan";
	case ComponentEnum::NativeScript:
		return "Native Script";
	case ComponentEnum::Rectangle:
		return "Rectangle";
	case ComponentEnum::Score:
		return "Score";
	case ComponentEnum::State:
		return "State";
	case ComponentEnum::Transform:
		return "Transform";
	default:
		return "Unknown";
	}
}

const char* const* getComponentNames() {
	static std::array<std::string, COMPONENTS.size()> names;
	static bool initialized = false;

	if (!initialized) {
		for (size_t i = 0; i < COMPONENTS.size(); ++i) {
			names[i] = getComponentName(COMPONENTS[i]);
		}
		initialized = true;
	}

	static const char* ptrs[12];
	for (size_t i = 0; i < 12; ++i) {
		ptrs[i] = names[i].c_str();
	}

	return ptrs;
}

void addComponentByEnum(std::shared_ptr<Entity> entity, ComponentEnum comp) {
	switch (comp) {
	case ComponentEnum::Animation:
		entity->add<CAnimation>();
		break;
	case ComponentEnum::BoundingBox:
		entity->add<CBoundingBox>();
		break;
	case ComponentEnum::BoundingCircle:
		entity->add<CBoundingCircle>();
		break;
	case ComponentEnum::Circle:
		entity->add<CCircle>();
		break;
	case ComponentEnum::Gravity:
		entity->add<CGravity>();
		break;
	case ComponentEnum::Input:
		entity->add<CInput>();
		break;
	case ComponentEnum::Label:
		entity->add<CLabel>();
		break;
	case ComponentEnum::Lifespan:
		entity->add<CLifespan>();
		break;
	case ComponentEnum::Rectangle:
		entity->add<CRectangle>();
		break;
	case ComponentEnum::Score:
		entity->add<CScore>();
		break;
	case ComponentEnum::State:
		entity->add<CState>();
		break;
	case ComponentEnum::Transform:
		entity->add<CTransform>();
		break;
	}
}
