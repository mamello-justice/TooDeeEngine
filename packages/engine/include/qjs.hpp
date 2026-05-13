#pragma once

#include <limits>

#include <SFML/Graphics.hpp>

#include "quickjs.h"

#include "TooDeeCore.hpp"
#include "Components.hpp"
#include "Entity.hpp"
#include "GameEngine.hpp"

// ================================= Primitive =================================
JSValue JS_GetValueOf(JSContext* ctx, const float& val);
void JS_UpdateFromValue(JSContext* ctx, JSValue& val, float& result);

JSValue JS_GetValueOf(JSContext* ctx, const unsigned int& val);
void JS_UpdateFromValue(JSContext* ctx, JSValue& val, unsigned int& result);

JSValue JS_GetValueOf(JSContext* ctx, const int& val);
void JS_UpdateFromValue(JSContext* ctx, JSValue& val, int& result);

// =================================== Core ====================================
JSValue JS_GetValueOf(JSContext* ctx, const Vec2f& val);
void JS_UpdateFromValue(JSContext* ctx, JSValue& val, Vec2f& result);

JSValue JS_GetValueOf(JSContext* ctx, const Vec2u& val);
void JS_UpdateFromValue(JSContext* ctx, JSValue& val, Vec2u& result);

JSValue JS_GetValueOf(JSContext* ctx, const Vec2i& val);
void JS_UpdateFromValue(JSContext* ctx, JSValue& val, Vec2i& result);

// =================================== SFML ====================================
JSValue JS_GetValueOf(JSContext* ctx, const sf::RenderTarget& val);

// ================================== Engine ===================================
JSValue JS_GetValueOf(JSContext* ctx, const CTransform& val);
void JS_UpdateFromValue(JSContext* ctx, JSValue& val, CTransform& result);

JSValue JS_GetValueOf(JSContext* ctx, const CBoundingBox& val);
void JS_UpdateFromValue(JSContext* ctx, JSValue& val, CBoundingBox& result);

JSValue JS_GetValueOf(JSContext* ctx, const CBoundingCircle& val);
void JS_UpdateFromValue(JSContext* ctx, JSValue& val, CBoundingCircle& result);

JSValue JS_GetValueOf(JSContext* ctx, const Entity& val);
void JS_UpdateFromValue(JSContext* ctx, JSValue& val, Entity& result);

JSValue JS_GetValueOf(JSContext* ctx, const GameEngine& val);
void JS_UpdateFromValue(JSContext* ctx, JSValue& val, GameEngine& result);
