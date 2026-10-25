#include "EntityManager.hpp"

#include <memory>
#include <map>
#include <limits>
#include <string>
#include <stdexcept>
#include <vector>

#include "Entity.hpp"

void EntityManager::removeDeadEntities(EntityVec& vec) {
	std::vector<size_t> toRemove;
	for (size_t i = vec.size(); i > 0; i--) {
		auto& e = vec.at(i - 1);
		if (!e->isAlive()) {
			toRemove.push_back(i - 1);
		}
	}

	for (auto i : toRemove) {
		vec.erase(vec.begin() + i);
	}
}

void EntityManager::update() {
	for (auto& e : mEntitiesToAdd) {
		mEntities.push_back(e);
		mEntityMap[e->tag()].push_back(e);
	}

	mEntitiesToAdd.clear();

	removeDeadEntities(mEntities);

	// remove dead entities from each vector in the entity map
	for (auto& [tag, entityVec] : mEntityMap) {
		removeDeadEntities(entityVec);
	}
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) {
	auto entity = std::shared_ptr<Entity>(new Entity(mTotalEntities++, tag));
	mEntitiesToAdd.push_back(entity);
	return entity;
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag, size_t id) {
	if (id == std::numeric_limits<size_t>::max()) {
		throw std::overflow_error("Entity ID is too large");
	}
	if (id >= mTotalEntities) mTotalEntities = id + 1;
	auto entity = std::shared_ptr<Entity>(new Entity(id, tag));
	mEntitiesToAdd.push_back(entity);
	return entity;
}

const EntityVec& EntityManager::getEntities() const {
	return mEntities;
}

EntityVec EntityManager::getAllEntities() const {
	EntityVec entities = mEntities;
	entities.insert(entities.end(), mEntitiesToAdd.begin(), mEntitiesToAdd.end());
	return entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag) {
	return mEntityMap[tag];
}

const EntityMap& EntityManager::getEntityMap() const {
	return mEntityMap;
}
