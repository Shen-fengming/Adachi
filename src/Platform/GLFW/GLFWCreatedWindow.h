#pragma once

#include "Adachi/Core/Window.h"
#include "Adachi/Diagnostics/Log.h"
#include "Adachi/Renderer/GraphicsAPI.h"
#include "Adachi/Event/Event.h"
#include <cstdint>

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#include <string>


namespace Adachi{

class GLFWCreatedWindow: public Window{
public:
    GLFWCreatedWindow(const Window::Config& config);
    virtual ~GLFWCreatedWindow() override;
    void PollEvents() override;

    void* GetNativeHandle() const override{
        return m_Window;
    };
    
    std::pair<uint32_t, uint32_t> GetFramebufferSize() const override 
    {
        return { m_FramebufferWidth, m_FramebufferHeight};
    }
    void SetFramebufferSize(uint32_t width, uint32_t height) override
    {
        m_FramebufferHeight = height;
        m_FramebufferWidth = width;
    }
    void SetEventCallbacks(const std::function<void(Event&)> callback) override
    {
        m_Callback = std::move(callback);
    };

    inline static void GLFWErrorCallback(int error, const char* description)
	{
		ADACHI_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

private:
    void Init(const Window::Config& config);
    void Shutdown();
    void GlfwInit();
    void CreateGLFWwindow();
    void SetCallbacks();
    //void SetVSync(bool enabled);
private:
    std::string m_Title;
    uint32_t m_Height;
    uint32_t m_Width;
    int32_t m_FramebufferHeight;
    int32_t m_FramebufferWidth;
    GraphicsAPI m_Graphics; // Different api has different implementation details

    std::function<void(Event&)> m_Callback;
    
    GLFWwindow* m_Window;
};

}