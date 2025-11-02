#include "Entity.hpp"

#include <string>
#include <tuple>

#include "Components.hpp"

Entity::Entity(const size_t& id, const std::string& tag) : mId(id), mTag(tag) {}

size_t Entity::id() const {
	return mId;
}

bool Entity::isAlive() const {
	return mAlive;
}

void Entity::destroy() {
	mAlive = false;
}

const std::string& Entity::tag() const {
	return mTag;
}
