#include "pch.h"
#include "Entity.h"

#include "Core/Application.h"

namespace Prism {
	Entity::Entity()
	{
		id = Application::world->m_GeneratorID++;
		Application::world->m_Entities[id] = this;
	}

	Entity::~Entity()
	{
		Application::world->DestroyComponents(this);
	}

	void Entity::Register(Component* addedComponent)
	{
		Application::world->m_Components.emplace_back(addedComponent);
	}
}