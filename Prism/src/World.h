#pragma once

#include "Entity.h"

namespace Prism {

	class World
	{
	public:
		Entity* GetEntity(EntityID id)
		{
			auto itr = m_Entities.find(id);
			if (itr != m_Entities.end())
				return itr->second;
			PR_CORE_WARN("Could not find Entity with id {0}", id);
			return nullptr;
		}

	private:
		friend class Entity;

		std::atomic<EntityID> m_GeneratorID{ 1 };
		std::unordered_map<EntityID, Entity*> m_Entities{};
		std::vector<std::unique_ptr<Component>> m_Components{};

		void DestroyComponents(Entity* entity) 
		{
			PR_CORE_WARN("Components of destroyed Entity don't get destructed before World destruction.");
		}
	};
}