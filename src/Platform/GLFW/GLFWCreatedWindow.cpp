#include "GLFWCreatedWindow.h"
#include "Adachi/Diagnostics/Profiler.h"
#include "Adachi/Diagnostics/Log.h"
#include "Adachi/Diagnostics/Debug.h"
#include "Adachi/Renderer/GraphicsAPI.h"
#include "Adachi/Event/ApplicationEvent.h"
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

namespace Adachi{

GLFWCreatedWindow::GLFWCreatedWindow(const Window::Config& config)
{
    ADACHI_PROFILE_FUNCTION();

    Init(config);
}

GLFWCreatedWindow::~GLFWCreatedWindow(){

}

void GLFWCreatedWindow::PollEvents()
{
    glfwPollEvents();
}

#pragma region PrivateHelpers
void GLFWCreatedWindow::Init(const Window::Config& config)
{
    ADACHI_PROFILE_FUNCTION();

    m_Height = config.Height;
    m_Width = config.Width;
    m_Title = config.Title;
    m_Graphics = config.Graphics;

    ADACHI_CORE_INFO("Creating window {0}: {1} * {2}", config.Title, config.Height, config.Width);

    GlfwInit();

    CreateGLFWwindow();

    SetCallbacks();
}

void GLFWCreatedWindow::Shutdown(){
    ADACHI_PROFILE_FUNCTION();

    glfwDestroyWindow(m_Window);

    glfwTerminate();
}

void GLFWCreatedWindow::GlfwInit()
{
    ADACHI_PROFILE_FUNCTION();

    int success = glfwInit();

    ADACHI_CORE_ASSERT(success, "Can not open GLFW!");

    glfwSetErrorCallback(GLFWErrorCallback);

    switch(m_Graphics){
        case GraphicsAPI::OpenGL:
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            

            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#ifdef __APPLE__
            glfwWindowHint(
                GLFW_OPENGL_FORWARD_COMPAT,
                GLFW_TRUE
            );
#endif
            break;
        default:
            ADACHI_CORE_ASSERT(false, "Can not find available graphics api type for GLFW!");
    }
}

void GLFWCreatedWindow::CreateGLFWwindow()
{
    ADACHI_PROFILE_FUNCTION();
    
    m_Window = glfwCreateWindow((int)m_Width, (int)m_Height, m_Title.c_str(), nullptr, nullptr);

    if(!m_Window){
        ADACHI_CORE_ASSERT(false, "Can't create GLFW window to initialize OpenGL context!");
    }

    glfwMakeContextCurrent(m_Window);

    glfwGetFramebufferSize(m_Window, &m_FramebufferWidth, &m_FramebufferHeight);
    ADACHI_CORE_INFO("Framebuffer: width: {}, height {}", m_FramebufferWidth, m_FramebufferHeight);
}

void GLFWCreatedWindow::SetCallbacks(){
    ADACHI_PROFILE_FUNCTION();

    glfwSetWindowUserPointer(m_Window, &m_Callback);

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window){
        std::function<void(Event&)>& callback = *(std::function<void(Event&)>*)glfwGetWindowUserPointer(window);
        WindowCloseEvent event;
        callback(event);
        //ADACHI_CORE_INFO("Callback issued");
    });

    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height){
        std::function<void(Event&)>& callback = *(std::function<void(Event&)>*)glfwGetWindowUserPointer(window);
        WindowResizeEvent event(width, height);
        callback(event);
    });

    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height){
        std::function<void(Event&)>& callback = *(std::function<void(Event&)>*)glfwGetWindowUserPointer(window);
        FramebufferResizeEvent event(width, height);
        callback(event);
    });
}

#pragma endregion
}