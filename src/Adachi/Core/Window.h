#pragma once

#include "Adachi/Renderer/GraphicsAPI.h"
#include "Adachi/Event/Event.h"

#include <cstdint>
#include <string>
#include <memory>
#include <functional>

namespace Adachi{

class Window{
public:
    struct Config{
        std::string Title;
        uint32_t Width;
        uint32_t Height;
        GraphicsAPI Graphics;

        Config(const std::string& title = "Unamed Window",
                GraphicsAPI graphics = GraphicsAPI::OpenGL,
                uint32_t width = 1600, 
                uint32_t height = 900
            )
            :Title(title),Width(width), Height(height), Graphics(graphics)
            {};
    };
public:
    virtual ~Window() = default;

    virtual void PollEvents() = 0;

    // Provide the pointer to original window thing to consturct
    virtual void* GetNativeHandle() const = 0;
    virtual std::pair<uint32_t, uint32_t> GetFramebufferSize() const = 0;
    virtual void SetFramebufferSize(uint32_t, uint32_t) = 0;

    virtual void SetEventCallbacks(const std::function<void(Event&)> callback) = 0;

    static std::unique_ptr<Window> Create(const Window::Config& config);
};


}