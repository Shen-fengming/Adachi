#pragma once

#include "Event.h"

#include <cstdint>


namespace Adachi {

class WindowResizeEvent: public Event{
public:
    WindowResizeEvent(uint32_t height, uint32_t width):m_Height(height), m_Width(width){}

    uint32_t GetHeight(){return m_Height;}
    uint32_t GetWidth(){return m_Width;}

    DERIVED_EVENT_TYPE(WindowResize);
private:
    uint32_t m_Height;
    uint32_t m_Width;
};

class FramebufferResizeEvent: public Event{
public:
    FramebufferResizeEvent(uint32_t height, uint32_t width):m_Height(height), m_Width(width){}

    uint32_t GetHeight(){return m_Height;}
    uint32_t GetWidth(){return m_Width;}

    DERIVED_EVENT_TYPE(WindowResize);
private:
    uint32_t m_Height;
    uint32_t m_Width;
};

class WindowCloseEvent: public Event{
public:
    WindowCloseEvent(){}

    DERIVED_EVENT_TYPE(WindowClose);
private:
};

}