#pragma once

#include "Components/Component.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"

#include <typeinfo>
#include <typeindex>
#include <map>

namespace Prism {

	using EntityID = uint32_t;
	
	class Entity
	{
	public:
		EntityID id;

		Entity();
		~Entity();

	private:
		// Wraps addedComponent (allocated with new) in a unique_ptr in Application::world
		void Register(Component* addedComponent);

	public:
		std::multimap<const std::type_info*, Component*> components;

		template<typename T, typename ...Args>
		constexpr T* AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of<Component, T>::value, "Component classes must be derivatives of Component.");
			T* result = new T(std::forward<Args>(args)...);
			Register(result);
			components.insert(std::pair(&typeid(T), dynamic_cast<Component*>(result)));
			return result;
		}

		template<typename T>
		constexpr T* Get()
		{
			const auto itr = components.find(&typeid(T));
			PR_CORE_ASSERT(itr != components.end(), "Component classes must be derivatives of Component.");
			return dynamic_cast<T*>(itr->second);
		}
	};
}