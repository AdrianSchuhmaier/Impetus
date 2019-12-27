#pragma once

#include <functional>

#define PR_BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

namespace Prism {

	class Event
	{
	public:
		enum class Type {
			WindowClose, WindowResize,
			KeyPressed, KeyReleased,
			MouseMoved, MouseScrolled,
			MouseButtonPressed, MouseButtonReleased
		};
		virtual Type GetType() const = 0;

		// Try to handle event as type T
		// Returns false if T is the wrong type
		// Otherwise, the event is handled with function f
		template<typename T, typename F>
		bool Handle(const F& f)
		{
			if (GetType() != T::GetStaticType())
				return false;

			handled = f(static_cast<T&>(*this));
			return true;
		}

		bool handled = false;
	};
	
	// Define Static + Dynamic Type for check in Dispatch
#define DEFINE_EVENT_TYPE(type)\
	static Event::Type GetStaticType() { return type; }\
	virtual Event::Type GetType() const override { return type; }


	// ========= Window Events =========================================

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;
		DEFINE_EVENT_TYPE(Type::WindowClose)
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height)
			: m_Width(width), m_Height(height) {}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		DEFINE_EVENT_TYPE(Type::WindowResize)
	private:
		uint32_t m_Width, m_Height;
	};


	// ========= Other Events ==========================================
	// TODO
}
