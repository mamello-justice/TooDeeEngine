#include "Physics.hpp"

#include <memory>

#include "Components.hpp"
#include "Core.hpp"
#include "Entity.hpp"

namespace Physics
{
	Vec2f GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity>b) {
		Vec2f delta(
			abs(a->get<CTransform>().pos.x - b->get<CTransform>().pos.x),
			abs(a->get<CTransform>().pos.y - b->get<CTransform>().pos.y));
		return a->get<CBoundingBox>().halfSize + b->get<CBoundingBox>().halfSize - delta;
	}

	Vec2f GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity>b) {
		Vec2f delta(
			abs(a->get<CTransform>().prevPos.x - b->get<CTransform>().prevPos.x),
			abs(a->get<CTransform>().prevPos.y - b->get<CTransform>().prevPos.y));
		return a->get<CBoundingBox>().halfSize + b->get<CBoundingBox>().halfSize - delta;
	}


	bool CircleCollision(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
		auto sumRadii = a->get<CBoundingCircle>().radius + b->get<CBoundingCircle>().radius;
		return a->isAlive() && b->isAlive() && a->get<CTransform>().pos.distSq(b->get<CTransform>().pos) <= sumRadii * sumRadii;
	}
} // namespace Physics
