#pragma once

#include <glm/glm.hpp>
#include <string>
#include <ranges>

#include "Adachi/Diagnostics/Log.h"
#include "Adachi/Event/Event.h"
#include "Adachi/Renderer/Renderable.h"
#include "Adachi/Scene/Camera.h"
#include "Adachi/Core/Window.h"

namespace Adachi {

struct ViewData {
  glm::mat4 view{1.0f};
  glm::mat4 projection{1.0f};
  //glm::mat4 viewProjection{1.0f};
};

struct RenderItem {
  const Adachi::Renderable &Renderable;
  glm::mat4 Transform;
};

class Scene {
public:
  Scene(const std::string &name) : m_Name(name){}
  virtual ~Scene() = default;

  virtual void OnAttach() = 0;
  virtual void OnDetach() = 0;
  virtual void OnEvent(Event &event) = 0;
  virtual void OnUpdate(float time) = 0;

public:
  const std::string &GetName() const { return m_Name; }

  const glm::mat4 GetViewMat() const{return m_Camera.GetViewMatrix();}
  const glm::mat4 GetProjectionMat() const{return m_Camera.GetProjectionMatrix();}

  auto GetRenderables() const {
    return m_Entities | 
    std::views::filter([](const auto &entity) 
    {
      return entity && entity->GetRenderable();
    }) 
    |
    std::views::transform([](const auto &entity) 
    {
      const auto &renderable = *entity->GetRenderable();


      //ADACHI_CORE_INFO("entity's transform: {},{},{}", entity->GetPosition().x,entity->GetPosition().y,entity->GetPosition().z);

      return RenderItem{.Renderable = renderable,
                        .Transform = entity->GetTransform() * renderable.Transform};
    });
  }
public:
  Window* imgui_window;
protected:
  Camera& GetCamera(){return m_Camera;}

protected:
  std::string m_Name;
  
  std::vector<std::unique_ptr<Entity>> m_Entities;

  Camera m_Camera;

  // Lights           container? <unique_ptr>
  // std::list<Light>

      // temp for imgui
};

} // namespace Adachi