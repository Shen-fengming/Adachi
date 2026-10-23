#include "Application.h"
#include "Adachi/Diagnostics/Log.h"
#include "Adachi/Event/ApplicationEvent.h"
#include "Adachi/Renderer/GraphicsAPI.h"
#include "Adachi/Diagnostics/Profiler.h"
#include "Adachi/Event/Event.h"
#include "Adachi/Resource/ResourceManager.h"
#include "Window.h"

#include <GLFW/glfw3.h>



namespace Adachi {
extern GraphicsAPI s_GraphicsAPI;

Application::Application(const Config &config) : m_Config(config) {
  ADACHI_PROFILE_FUNCTION();

  m_Window = Window::Create(Window::Config(m_Config.Name, s_GraphicsAPI));
  m_Window->SetEventCallbacks(
    [this](Event& event)
    {
      OnEvent(event);
    }
  );
  m_Renderer = Renderer::Create(*m_Window);
  m_Renderer->Init();
  m_Pipeline = std::make_unique<RenderPipeline>();

  m_ResourceManager = std::make_unique<ResourceManager>(ResourceManager::Config{std::filesystem::path{ADACHI_PROJECT_ROOT} / "asset"});
}

void Application::Run() {

  double previousTime = glfwGetTime();

  while (m_Running) {

    const double currentTime = glfwGetTime();
    const float deltaTime = static_cast<float>(currentTime - previousTime);
    previousTime = currentTime;

    m_Window->PollEvents();

    m_Scene->OnUpdate(deltaTime);

    m_Pipeline->Execute(*m_Renderer, *m_Scene);
  }
}

void Application::OnEvent(Event& event){
  ADACHI_PROFILE_FUNCTION();
  TypedEventHandler eventHandler(event);

  eventHandler.Handle<WindowCloseEvent>(
    [this](WindowCloseEvent& event)
    {
      return OnWindowClose(event);
    }
  );
  eventHandler.Handle<WindowResizeEvent>(
    [this](WindowResizeEvent& event)
    {
      return OnWindowResize(event);
    }
  );

  // Give event to ImGUI

  // Give event to Scene

}

bool Application::OnWindowClose(WindowCloseEvent& e){
  ADACHI_PROFILE_FUNCTION();

  m_Running = false;

  ADACHI_CORE_INFO("Window close event processed");

  return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e){
  ADACHI_PROFILE_FUNCTION();

  //m_Renderer->Resize(e.GetWidth(), e.GetHeight());

  return true;
}

bool Application::OnFramebufferResize(FramebufferResizeEvent& e){
  ADACHI_PROFILE_FUNCTION();

  m_Window->SetFramebufferSize(e.GetWidth(), e.GetHeight());
  m_Renderer->Resize(e.GetWidth(), e.GetHeight());

  return true;
}
} // namespace Adachi