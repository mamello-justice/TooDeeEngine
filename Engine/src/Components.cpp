#include "Components.hpp"

#include <SFML/Graphics.hpp>

#include "Animation.hpp"
#include "Vec2.hpp"

CTransform::CTransform(const Vec2f& p) : pos(p) {}

CTransform::CTransform(const Vec2f& p, const Vec2f& v, const Vec2f& s, float a)
	: pos(p), prevPos(p), velocity(v), scale(s), angle(a) {}

CShape::CShape(float radius, size_t points, const sf::Color& fill, const sf::Color& outline, float thickness)
	: circle(radius, points) {
	circle.setFillColor(fill);
	circle.setOutlineColor(outline);
	circle.setOutlineThickness(thickness);
	circle.setOrigin({ radius, radius });
}

CScore::CScore(int s) : score(s) {}

CLifespan::CLifespan(int totalLifespan)
	: lifespan(totalLifespan), remaining(totalLifespan) {}

CBoundingBox::CBoundingBox(const Vec2f& s) :
	size(s), halfSize(s.x / 2, s.y / 2) {}

CBoundingCircle::CBoundingCircle(float r) : radius(r) {}

CAnimation::CAnimation(const Animation& anim, bool r) :
	animation(anim), repeat(r) {}

CGravity::CGravity(float g) : gravity(g) {}

CState::CState(const std::string& s) : state(s) {}

