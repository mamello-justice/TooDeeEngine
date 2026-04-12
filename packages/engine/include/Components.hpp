#pragma once

#include <array>
#include <functional>
#include <memory>
#include <tuple>

#include <SFML/Graphics.hpp>

#include "Animation.hpp"
#include "Vec2.hpp"

class Entity;

class Component {
public:
	bool exists = false;
};

class CLabel : public Component {
public:
	std::string label = "";

	CLabel();
	CLabel(const std::string& l);
};

class CTransform : public Component {
public:
	Vec2f pos = { 0.0, 0.0 };
	Vec2f prevPos = { 0.0, 0.0 };
	Vec2f scale = { 1.0, 1.0 };
	Vec2f velocity = { 0.0, 0.0 };
	float angle = 0;

	CTransform() = default;
	CTransform(const Vec2f& p);
	CTransform(const Vec2f& p, const Vec2f& v);
	CTransform(const Vec2f& p, const Vec2f& v, const Vec2f& s, float a);
};

class CCircle : public Component {
public:
	size_t points = 50;
	float thickness = 1.0;
	float radius = 10.0;
	sf::Color fill = sf::Color::Black;
	sf::Color outline = sf::Color::White;

	CCircle() = default;
	CCircle(float radius, size_t points, const sf::Color& fill, const sf::Color& outline, float thickness);
};

class CRectangle : public Component {
public:
	float thickness = 1.0;
	Vec2f size = { 10.0, 10.0 };
	sf::Color fill = sf::Color::Black;
	sf::Color outline = sf::Color::White;

	CRectangle() = default;
	CRectangle(const Vec2f& size, const sf::Color& fill, const sf::Color& outline, float thickness);
};

class CScore : public Component {
public:
	int score = 0;

	CScore() = default;
	CScore(int s);
};

class CLifespan : public Component {
public:
	int lifespan = 0;
	int remaining = 0;
	CLifespan() = default;
	CLifespan(int totalLifespan);
};

class CInput : public Component {
public:
	bool up = false;
	bool left = false;
	bool right = false;
	bool down = false;
	bool shoot = false;
	bool canShoot = true;
	bool canJump = true;

	CInput() = default;
};

class CBoundingBox : public Component {
public:
	Vec2f size;
	Vec2f halfSize;

	CBoundingBox() = default;
	CBoundingBox(const Vec2f& s);
};

class CBoundingCircle : public Component {
public:
	float radius = 0;

	CBoundingCircle() = default;
	CBoundingCircle(float r);
};

class CAnimation : public Component {
public:
	Animation animation;
	bool repeat = false;

	CAnimation() = default;
	CAnimation(const Animation& anim, bool r);
};

class CGravity : public Component {
public:
	float gravity = 0;

	CGravity() = default;
	CGravity(float g);
};

class CState : public Component {
public:
	std::string state = "jumping";

	CState() = default;
	CState(const std::string& s);
};

class CNativeScript : public Component {
public:
	std::function<void(Entity&)> onUpdate;

	CNativeScript() = default;
	CNativeScript(const std::function<void(Entity&)>& updateFunc);
};

using ComponentTuple = std::tuple<
	CAnimation,
	CBoundingBox,
	CBoundingCircle,
	CCircle,
	CGravity,
	CInput,
	CLabel,
	CLifespan,
	CNativeScript,
	CRectangle,
	CScore,
	CState,
	CTransform>;

enum class ComponentEnum {
	Animation,
	BoundingBox,
	BoundingCircle,
	Circle,
	Gravity,
	Input,
	Label,
	Lifespan,
	NativeScript,
	Rectangle,
	Score,
	State,
	Transform
};

constexpr std::array<ComponentEnum, 13> COMPONENTS = {
	ComponentEnum::Animation,
	ComponentEnum::BoundingBox,
	ComponentEnum::BoundingCircle,
	ComponentEnum::Circle,
	ComponentEnum::Gravity,
	ComponentEnum::Input,
	ComponentEnum::Label,
	ComponentEnum::Lifespan,
	ComponentEnum::NativeScript,
	ComponentEnum::Rectangle,
	ComponentEnum::Score,
	ComponentEnum::State,
	ComponentEnum::Transform
};

std::string getComponentName(ComponentEnum c);

const char* const* getComponentNames();

void addComponentByEnum(std::shared_ptr<Entity> entity, ComponentEnum comp);

inline constexpr int getComponentCount() {
	return static_cast<int>(COMPONENTS.size());
}

template<typename EntityType>
bool hasComponentByEnum(const EntityType& entity, ComponentEnum comp) {
	switch (comp) {
	case ComponentEnum::Animation:
		return entity.template has<CAnimation>();
	case ComponentEnum::BoundingBox:
		return entity.template has<CBoundingBox>();
	case ComponentEnum::BoundingCircle:
		return entity.template has<CBoundingCircle>();
	case ComponentEnum::Circle:
		return entity.template has<CCircle>();
	case ComponentEnum::Gravity:
		return entity.template has<CGravity>();
	case ComponentEnum::Input:
		return entity.template has<CInput>();
	case ComponentEnum::Label:
		return entity.template has<CLabel>();
	case ComponentEnum::Lifespan:
		return entity.template has<CLifespan>();
	case ComponentEnum::NativeScript:
		return entity.template has<CNativeScript>();
	case ComponentEnum::Rectangle:
		return entity.template has<CRectangle>();
	case ComponentEnum::Score:
		return entity.template has<CScore>();
	case ComponentEnum::State:
		return entity.template has<CState>();
	case ComponentEnum::Transform:
		return entity.template has<CTransform>();
	default:
		return false;
	}
}

static_assert(std::is_default_constructible_v<CAnimation>);
static_assert(std::is_default_constructible_v<CBoundingBox>);
static_assert(std::is_default_constructible_v<CBoundingCircle>);
static_assert(std::is_default_constructible_v<CCircle>);
static_assert(std::is_default_constructible_v<CGravity>);
static_assert(std::is_default_constructible_v<CInput>);
static_assert(std::is_default_constructible_v<CLabel>);
static_assert(std::is_default_constructible_v<CLifespan>);
static_assert(std::is_default_constructible_v<CNativeScript>);
static_assert(std::is_default_constructible_v<CRectangle>);
static_assert(std::is_default_constructible_v<CScore>);
static_assert(std::is_default_constructible_v<CState>);
static_assert(std::is_default_constructible_v<CTransform>);
