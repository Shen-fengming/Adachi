#pragma once

namespace Adachi {
enum class EventType{
    None = 0,
    WindowClose, WindowResize
};
#define DERIVED_EVENT_TYPE(type)                                  \
    static constexpr EventType GetStaticType() noexcept { \
        return EventType::type;                           \
    }                                                     \
    EventType GetType() const noexcept override {         \
        return GetStaticType();                           \
    }

class Event{
public:
    virtual ~Event() = default;

    void Handled(bool success){m_Handled |= success;}

    static constexpr EventType GetStaticType() noexcept 
    { 
        return EventType::None;                           
    }                                                     
    virtual EventType GetType() const noexcept  
    { 
        return GetStaticType();                          
    }
private:
    bool m_Handled = false;
};

class TypedEventHandler{
public:
    TypedEventHandler(Event& event):m_Event(event){};

    template<class DerivedEvent, class Func>
    bool Handle(const Func& func){
        if(m_Event.GetType() == DerivedEvent::GetStaticType()){
            m_Event.Handled(func(static_cast<DerivedEvent&>(m_Event)));

            return true;
        }

        return false;
    }

private:
    Event& m_Event;
};

}