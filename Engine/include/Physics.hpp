#pragma once

#include <memory>

#include "Core.hpp"
#include "Entity.hpp"

namespace Physics
{
	Vec2f GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);

	Vec2f GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);

	bool CircleCollision(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
} // namespace Physics
