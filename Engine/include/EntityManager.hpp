#pragma once

#include <memory>
#include <map>
#include <string>
#include <vector>

#include "Entity.hpp"

using EntityVec = std::vector<std::shared_ptr<Entity>>;
using EntityMap = std::map<std::string, EntityVec>;

class EntityManager {
	EntityVec	mEntities;
	EntityVec	mEntitiesToAdd;
	EntityMap	mEntityMap;
	size_t		mTotalEntities = 0;

	void removeDeadEntities(EntityVec& vec);

public:
	EntityManager() = default;

	void update();

	std::shared_ptr<Entity> addEntity(const std::string& tag);

	const EntityVec& getEntities() const;

	const EntityVec& getEntities(const std::string& tag);

	const EntityMap& getEntityMap() const;
};
