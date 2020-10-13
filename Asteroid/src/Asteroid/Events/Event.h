#pragma once

#include "Asteroid/Core.h"

//Base event class that serves as interface for other events
//Macros EVENT_CLASS_TYPE and EVENT_CLASS_CATEGORY defined to 
//provide quick functions to categorize various events

namespace Asteroid {
//Event blocking system, meaning that events are immediately dispatched
//and dealt with right away. May swap to buffer events in an event bus

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication    = BIT(0),
		EventCategoryInput          = BIT(1),
		EventCategoryKeyboard		= BIT(2),
		EventCategoryMouse			= BIT(3),
		EventCategoryMouseButton	= BIT(4)
	};

//Need static type because at runtime, we need to check what each event type is
//Static because event type is tied to class, not each instance of class
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() {return EventType::##type;}\
								virtual EventType GetEventType() const override {return GetStaticType();}\
								virtual const char* GetName() const override {return #type;}

//Macro that defines function to categorize event in corresponding EventCategory
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override {return category;}

	class ASTEROID_API Event
	{
	public: 
		bool Handled = false;
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	};

//EventDispatcher to dispatch/handle events 
//If event type passed to EventDispatcher matches the type specified by template T
//EventDispatcher casts m_Event to type T and executes the callback fn
//passed to EventDispatcher's Dispatch method
	class EventDispatcher
	{
		template<typename T> 
		using EventFn = std::function<bool(T&)>;
	public: 
		EventDispatcher(Event& event) : m_Event(event) {}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private: 
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}