#pragma once

#include <SFML/Graphics.hpp>

#include "Animation.hpp"
#include "Vec2.hpp"

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
	sf::CircleShape circle;

	CCircle() = default;
	CCircle(float radius, size_t points, const sf::Color& fill, const sf::Color& outline, float thickness);
};

class CRectangle : public Component {
public:
	sf::RectangleShape rect;

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

static_assert(std::is_default_constructible_v<CAnimation>);
static_assert(std::is_default_constructible_v<CBoundingBox>);
static_assert(std::is_default_constructible_v<CBoundingCircle>);
static_assert(std::is_default_constructible_v<CCircle>);
static_assert(std::is_default_constructible_v<CGravity>);
static_assert(std::is_default_constructible_v<CInput>);
static_assert(std::is_default_constructible_v<CLabel>);
static_assert(std::is_default_constructible_v<CLifespan>);
static_assert(std::is_default_constructible_v<CRectangle>);
static_assert(std::is_default_constructible_v<CScore>);
static_assert(std::is_default_constructible_v<CState>);
static_assert(std::is_default_constructible_v<CTransform>);
