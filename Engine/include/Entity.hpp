#pragma once

#include <string>
#include <tuple>

#include "Components.hpp"

class EntityManager;

using ComponentTuple = std::tuple<
	CAnimation,
	CBoundingBox,
	CBoundingCircle,
	CGravity,
	CInput,
	CLifespan,
	CScore,
	CShape,
	CState,
	CTransform>;

class Entity
{
	friend class EntityManager;

	ComponentTuple mComponents;

	size_t mId = (size_t)0;
	std::string mTag = "default";
	bool mAlive = true;
	Entity() {}
	Entity(const size_t &id, const std::string &tag);

public:
	template <typename T, typename... TArgs>
	T &add(TArgs &&...args)
	{
		auto &component = get<T>();
		component = T(std::forward<TArgs>(args)...);
		component.exists = true;
		return component;
	}

	template <typename T>
	T &get()
	{
		return std::get<T>(mComponents);
	}

	template <typename T>
	const T &get() const
	{
		return std::get<T>(mComponents);
	}

	template <typename T>
	bool has() const
	{
		return get<T>().exists;
	}

	template <typename T>
	void remove()
	{
		get<T>() = T();
	}

	size_t id() const;

	bool isAlive() const;

	void destroy();

	const std::string &tag() const;
};
