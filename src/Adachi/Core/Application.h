#pragma once

//#include "Adachi/Util/ResourceManager.h"
#include "Adachi/Renderer/RenderPipeline.h"
#include "Window.h"
#include "Scene.h"
#include "Adachi/Renderer/Renderer.h"
#include "Adachi/Event/Event.h"
#include "Adachi/Event/ApplicationEvent.h"

#include <string>


namespace Adachi{

class Application{
public:
    struct CommandLineArg{
        int argc = 0;
        char** argv = nullptr;
    };
    struct Config{
        std::string Name = "Unnamed App";
        std::string WorkingDirectory;
        CommandLineArg CommandLineArg;
    };

    Application(const Config& config);
    virtual ~Application() = default;

    void Run();
    void OnEvent(Event& event);
private:
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);
    bool OnFramebufferResize(FramebufferResizeEvent& e);
protected:
    Config m_Config;

    bool m_Running = true;

    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Renderer> m_Renderer;
    std::unique_ptr<RenderPipeline> m_Pipeline;
    //std::unique_ptr<ResourceManager> m_ResourceManager;
    std::unique_ptr<Scene> m_Scene;
};

Application* CreateApplication(Adachi::Application::Config config);

Application::Config CreateApplicationConfig(Application::CommandLineArg commmandLineArg);
}