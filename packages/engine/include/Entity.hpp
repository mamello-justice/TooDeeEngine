#pragma once

#include <string>
#include <tuple>

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
#include <quickjspp.h>
#endif

#include "Components.hpp"

class EntityManager;

class Entity
{
	friend class EntityManager;

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
	friend struct qjs::js_traits<Entity>;
#endif

	ComponentTuple mComponents;

	size_t mId = (size_t)0;
	std::string mTag = "default";
	bool mAlive = true;
	Entity() {}
	Entity(const size_t& id, const std::string& tag);

public:
	template <typename T, typename... TArgs>
	T& add(TArgs &&...args) {
		auto& component = get<T>();
		component = T(std::forward<TArgs>(args)...);
		component.exists = true;
		return component;
	}

	template <typename T>
	T& get() {
		return std::get<T>(mComponents);
	}

	template <typename T>
	const T& get() const {
		return std::get<T>(mComponents);
	}

	template <typename T>
	bool has() const {
		return get<T>().exists;
	}

	template <typename T>
	void remove() {
		get<T>() = T();
	}

	size_t id() const;

	bool isAlive() const;

	void destroy();

	const std::string& tag() const;

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
	void operator()(JSContext* ctx, const JSValue& entity);
#endif
};

#ifdef TOO_DEE_ENGINE_QJS_SCRIPTING
namespace qjs
{
	/** Conversion traits for Entity
	 */
	template<>
	struct js_traits<Entity>
	{
		static Entity unwrap(JSContext* ctx, JSValueConst val) {
			auto v = js_traits<qjs::value>::unwrap(ctx, val);

			auto components = v["components"];

			Entity result;

			try {
				auto transform = components["transform"];
			}
			catch (...) {

			}

			return result;
		}
		static JSValue wrap(JSContext* ctx, const Entity& e) noexcept {
			qjs::context context(ctx);
			auto result = context.new_object();

			auto components = context.new_object();

			if (e.has<CTransform>()) {
				auto transform = context.new_object();

				auto& cTrans = e.get<CTransform>();
				transform["pos"] = js_traits<Vec2f>::wrap(ctx, cTrans.pos);
				transform["prevPos"] = js_traits<Vec2f>::wrap(ctx, cTrans.prevPos);
				transform["scale"] = js_traits<Vec2f>::wrap(ctx, cTrans.scale);
				transform["velocity"] = js_traits<Vec2f>::wrap(ctx, cTrans.velocity);
				transform["angle"] = js_traits<float>::wrap(ctx, cTrans.angle);

				components["transform"] = transform;
			}

			if (e.has<CBoundingBox>()) {

			}

			if (e.has<CBoundingCircle>()) {

			}

			result["components"] = components;
			return result.release();
		}
	};
} // namespace qjs
#endif
