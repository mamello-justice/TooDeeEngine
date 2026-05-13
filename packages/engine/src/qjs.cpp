#include "qjs.hpp"

#include <limits>

#include <SFML/Graphics.hpp>

#include "quickjs.h"

#include "TooDeeCore.hpp"
#include "Components.hpp"
#include "Entity.hpp"
#include "GameEngine.hpp"

// ================================= Primitive =================================
JSValue JS_GetValueOf(JSContext* ctx, const float& val) {
    return JS_NewFloat64(ctx, val);
}

void JS_UpdateFromValue(JSContext* ctx, JSValue& val, float& result) {
    double temp;
    if (JS_ToFloat64(ctx, &temp, val)) {
        throw std::runtime_error("Failed to convert JSValue to Float64");
    }

    if (std::abs(temp) > std::numeric_limits<float>::max()) {
        throw std::runtime_error("Converted JSValue is larger than float");
    }

    result = static_cast<float>(temp);
}

JSValue JS_GetValueOf(JSContext* ctx, const unsigned int& val) {
    return JS_NewUint32(ctx, val);
}

void JS_UpdateFromValue(JSContext* ctx, JSValue& val, unsigned int& result) {
    if (JS_ToUint32(ctx, &result, val)) {
        throw std::runtime_error("Failed to convert JSValue to Uint32");
    }
}

JSValue JS_GetValueOf(JSContext* ctx, const int& val) {
    return JS_NewInt32(ctx, val);
}

void JS_UpdateFromValue(JSContext* ctx, JSValue& val, int& result) {
    if (JS_ToInt32(ctx, &result, val)) {
        throw std::runtime_error("Failed to convert JSValue to Uint32");
    }
}

// =================================== Core ====================================
JSValue JS_GetValueOf(JSContext* ctx, const Vec2f& val) {
    JSValue result = JS_NewObject(ctx);

    JSValue x = JS_GetValueOf(ctx, val.x);
    JS_SetPropertyStr(ctx, result, "x", JS_DupValue(ctx, x));
    JS_FreeValue(ctx, x);

    JSValue y = JS_GetValueOf(ctx, val.y);
    JS_SetPropertyStr(ctx, result, "y", JS_DupValue(ctx, y));
    JS_FreeValue(ctx, y);

    return result;
}

void JS_UpdateFromValue(JSContext* ctx, JSValue& val, Vec2f& result) {
    JSValue x = JS_GetPropertyStr(ctx, val, "x");
    if (JS_IsException(x)) {
        throw std::runtime_error("Failed to get x property");
    }
    if (!JS_IsUndefined(x)) {
        JS_UpdateFromValue(ctx, x, result.x);
    }
    JS_FreeValue(ctx, x);

    JSValue y = JS_GetPropertyStr(ctx, val, "y");
    if (JS_IsException(y)) {
        throw std::runtime_error("Failed to get y property");
    }
    if (!JS_IsUndefined(y)) {
        JS_UpdateFromValue(ctx, y, result.y);
    }
    JS_FreeValue(ctx, y);
}

JSValue JS_GetValueOf(JSContext* ctx, const Vec2u& val) {
    JSValue result = JS_NewObject(ctx);

    JSValue x = JS_GetValueOf(ctx, val.x);
    JS_SetPropertyStr(ctx, result, "x", JS_DupValue(ctx, x));
    JS_FreeValue(ctx, x);

    JSValue y = JS_GetValueOf(ctx, val.y);
    JS_SetPropertyStr(ctx, result, "y", JS_DupValue(ctx, y));
    JS_FreeValue(ctx, y);

    return result;
}

void JS_UpdateFromValue(JSContext* ctx, JSValue& val, Vec2u& result) {
    JSValue x = JS_GetPropertyStr(ctx, val, "x");
    if (JS_IsException(x)) {
        throw std::runtime_error("Failed to get x property");
    }
    if (!JS_IsUndefined(x)) {
        JS_UpdateFromValue(ctx, x, result.x);
    }
    JS_FreeValue(ctx, x);

    JSValue y = JS_GetPropertyStr(ctx, val, "y");
    if (JS_IsException(y)) {
        throw std::runtime_error("Failed to get y property");
    }
    if (!JS_IsUndefined(y)) {
        JS_UpdateFromValue(ctx, y, result.y);
    }
    JS_FreeValue(ctx, y);
}

JSValue JS_GetValueOf(JSContext* ctx, const Vec2i& val) {
    JSValue result = JS_NewObject(ctx);

    JSValue x = JS_GetValueOf(ctx, val.x);
    JS_SetPropertyStr(ctx, result, "x", JS_DupValue(ctx, x));
    JS_FreeValue(ctx, x);

    JSValue y = JS_GetValueOf(ctx, val.y);
    JS_SetPropertyStr(ctx, result, "y", JS_DupValue(ctx, y));
    JS_FreeValue(ctx, y);

    return result;
}

void JS_UpdateFromValue(JSContext* ctx, JSValue& val, Vec2i& result) {
    JSValue x = JS_GetPropertyStr(ctx, val, "x");
    if (JS_IsException(x)) {
        throw std::runtime_error("Failed to get x property");
    }
    if (!JS_IsUndefined(x)) {
        JS_UpdateFromValue(ctx, x, result.x);
    }
    JS_FreeValue(ctx, x);

    JSValue y = JS_GetPropertyStr(ctx, val, "y");
    if (JS_IsException(y)) {
        throw std::runtime_error("Failed to get y property");
    }
    if (!JS_IsUndefined(y)) {
        JS_UpdateFromValue(ctx, y, result.y);
    }
    JS_FreeValue(ctx, y);
}

// =================================== SFML ====================================
JSValue JS_GetValueOf(JSContext* ctx, const sf::RenderTarget& val) {
    JSValue result = JS_NewObject(ctx);

    JSValue size = JS_GetValueOf(ctx, (Vec2u)val.getSize());
    JS_SetPropertyStr(ctx, result, "size", JS_DupValue(ctx, size));
    JS_FreeValue(ctx, size);

    return result;
}

// ================================== Engine ===================================
JSValue JS_GetValueOf(JSContext* ctx, const CTransform& val) {
    JSValue result = JS_NewObject(ctx);

    JSValue pos = JS_GetValueOf(ctx, val.pos);
    JS_SetPropertyStr(ctx, result, "pos", JS_DupValue(ctx, pos));
    JS_FreeValue(ctx, pos);

    JSValue prevPos = JS_GetValueOf(ctx, val.prevPos);
    JS_SetPropertyStr(ctx, result, "prevPos", JS_DupValue(ctx, prevPos));
    JS_FreeValue(ctx, prevPos);

    JSValue scale = JS_GetValueOf(ctx, val.scale);
    JS_SetPropertyStr(ctx, result, "scale", JS_DupValue(ctx, scale));
    JS_FreeValue(ctx, scale);

    JSValue velocity = JS_GetValueOf(ctx, val.velocity);
    JS_SetPropertyStr(ctx, result, "velocity", JS_DupValue(ctx, velocity));
    JS_FreeValue(ctx, velocity);

    JSValue angle = JS_GetValueOf(ctx, val.angle);
    JS_SetPropertyStr(ctx, result, "angle", JS_DupValue(ctx, angle));
    JS_FreeValue(ctx, angle);

    return result;
}

void JS_UpdateFromValue(JSContext* ctx, JSValue& val, CTransform& result) {
    JSValue pos = JS_GetPropertyStr(ctx, val, "pos");
    if (JS_IsException(pos)) {
        throw std::runtime_error("Failed to get pos property");
    }
    if (!JS_IsUndefined(pos)) {
        JS_UpdateFromValue(ctx, pos, result.pos);
    }
    JS_FreeValue(ctx, pos);

    JSValue prevPos = JS_GetPropertyStr(ctx, val, "prevPos");
    if (JS_IsException(prevPos)) {
        throw std::runtime_error("Failed to get prevPos property");
    }
    if (!JS_IsUndefined(prevPos)) {
        JS_UpdateFromValue(ctx, prevPos, result.prevPos);
    }
    JS_FreeValue(ctx, prevPos);

    JSValue scale = JS_GetPropertyStr(ctx, val, "scale");
    if (JS_IsException(scale)) {
        throw std::runtime_error("Failed to get scale property");
    }
    if (!JS_IsUndefined(scale)) {
        JS_UpdateFromValue(ctx, scale, result.scale);
    }
    JS_FreeValue(ctx, scale);

    JSValue velocity = JS_GetPropertyStr(ctx, val, "velocity");
    if (JS_IsException(velocity)) {
        throw std::runtime_error("Failed to get velocity property");
    }
    if (!JS_IsUndefined(velocity)) {
        JS_UpdateFromValue(ctx, velocity, result.velocity);
    }
    JS_FreeValue(ctx, velocity);

    JSValue angle = JS_GetPropertyStr(ctx, val, "angle");
    if (JS_IsException(angle)) {
        throw std::runtime_error("Failed to get angle property");
    }
    if (!JS_IsUndefined(angle)) {
        JS_UpdateFromValue(ctx, angle, result.angle);
    }
    JS_FreeValue(ctx, angle);
}

JSValue JS_GetValueOf(JSContext* ctx, const CBoundingBox& val) {
    JSValue result = JS_NewObject(ctx);

    JSValue size = JS_GetValueOf(ctx, val.size);
    JS_SetPropertyStr(ctx, result, "size", JS_DupValue(ctx, size));
    JS_FreeValue(ctx, size);

    JSValue halfSize = JS_GetValueOf(ctx, val.halfSize);
    JS_SetPropertyStr(ctx, result, "halfSize", JS_DupValue(ctx, halfSize));
    JS_FreeValue(ctx, halfSize);

    return result;
}

void JS_UpdateFromValue(JSContext* ctx, JSValue& val, CBoundingBox& result) {
    JSValue size = JS_GetPropertyStr(ctx, val, "size");
    if (JS_IsException(size)) {
        throw std::runtime_error("Failed to get size property");
    }
    if (!JS_IsUndefined(size)) {
        JS_UpdateFromValue(ctx, size, result.size);
    }
    JS_FreeValue(ctx, size);

    JSValue halfSize = JS_GetPropertyStr(ctx, val, "halfSize");
    if (JS_IsException(halfSize)) {
        throw std::runtime_error("Failed to get halfSize property");
    }
    if (!JS_IsUndefined(halfSize)) {
        JS_UpdateFromValue(ctx, halfSize, result.halfSize);
    }
    JS_FreeValue(ctx, halfSize);
}

JSValue JS_GetValueOf(JSContext* ctx, const CBoundingCircle& val) {
    JSValue result = JS_NewObject(ctx);

    JSValue radius = JS_GetValueOf(ctx, val.radius);
    JS_SetPropertyStr(ctx, result, "radius", JS_DupValue(ctx, radius));
    JS_FreeValue(ctx, radius);

    return result;
}

void JS_UpdateFromValue(JSContext* ctx, JSValue& val, CBoundingCircle& result) {
    JSValue radius = JS_GetPropertyStr(ctx, val, "radius");
    if (JS_IsException(radius)) {
        throw std::runtime_error("Failed to get radius property");
    }
    if (!JS_IsUndefined(radius)) {
        JS_UpdateFromValue(ctx, radius, result.radius);
    }
    JS_FreeValue(ctx, radius);
}

JSValue JS_GetValueOf(JSContext* ctx, const Entity& val) {
    JSValue result = JS_NewObject(ctx);

    JSValue components = JS_NewObject(ctx);

    if (val.has<CTransform>()) {
        JSValue comp = JS_GetValueOf(ctx, val.get<CTransform>());
        JS_SetPropertyStr(ctx, components, "transform", JS_DupValue(ctx, comp));
        JS_FreeValue(ctx, comp);
    }

    if (val.has<CBoundingBox>()) {
        JSValue comp = JS_GetValueOf(ctx, val.get<CBoundingBox>());
        JS_SetPropertyStr(ctx, components, "boundingBox", JS_DupValue(ctx, comp));
        JS_FreeValue(ctx, comp);
    }

    if (val.has<CBoundingCircle>()) {
        JSValue comp = JS_GetValueOf(ctx, val.get<CBoundingCircle>());
        JS_SetPropertyStr(ctx, components, "boundingCircle", JS_DupValue(ctx, comp));
        JS_FreeValue(ctx, comp);
    }

    JS_SetPropertyStr(ctx, result, "components", JS_DupValue(ctx, components));
    JS_FreeValue(ctx, components);

    return result;
}

void JS_UpdateFromValue(JSContext* ctx, JSValue& val, Entity& result) {
    JSValue components = JS_GetPropertyStr(ctx, val, "components");
    if (JS_IsException(components)) {
        throw std::runtime_error("Failed to get components property");
    }

    if (!JS_IsUndefined(components)) {
        JSValue transform = JS_GetPropertyStr(ctx, components, "transform");
        if (JS_IsException(transform)) {
            throw std::runtime_error("Failed to get transform property");
        }
        if (!JS_IsUndefined(transform)) {
            if (!result.has<CTransform>()) {
                result.add<CTransform>();
            }
            JS_UpdateFromValue(ctx, transform, result.get<CTransform>());
        }
        JS_FreeValue(ctx, transform);
    }

    JS_FreeValue(ctx, components);
}

JSValue JS_GetValueOf(JSContext* ctx, const GameEngine& val) {
    JSValue result = JS_NewObject(ctx);

    JSValue renderTarget = JS_GetValueOf(ctx, val.renderTarget());
    JS_SetPropertyStr(ctx, result, "renderTarget", JS_DupValue(ctx, renderTarget));
    JS_FreeValue(ctx, renderTarget);

    return result;
}

void JS_UpdateFromValue(JSContext* ctx, JSValue& val, GameEngine& result) {
    throw std::runtime_error("TODO");
}
