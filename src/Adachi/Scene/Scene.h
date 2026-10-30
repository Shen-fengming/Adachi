#pragma once

#include <glm/glm.hpp>
#include <string>
#include <ranges>

#include "Adachi/Diagnostics/Log.h"
#include "Adachi/Event/Event.h"
#include "Adachi/Renderer/Renderable.h"
#include "Adachi/Scene/Camera.h"
#include "Adachi/Scene/Entity.h"
#include "Adachi/Core/Window.h"
#include "Adachi/Resource/ResourceManager.h"

namespace Adachi {

struct ViewData {
  glm::mat4 view{1.0f};
  glm::mat4 projection{1.0f};
};

struct RenderItem {
  const Adachi::Renderable &Renderable;
  glm::mat4 Transform;
};

class Scene {
public:
  Scene(const std::string &name, ResourceManager& resourceManager, Window& window) : m_Name(name), m_ResourceManager(resourceManager), m_window(window){}
  virtual ~Scene() = default;

  virtual void OnAttach();
  virtual void OnDetach();
  virtual void OnEvent(Event &event) = 0;
  virtual void OnUpdate(float time);

public:
  const std::string &GetName() const { return m_Name; }

  const glm::mat4 GetViewMat() const{return m_Camera.GetViewMatrix();}
  const glm::mat4 GetProjectionMat() const{return m_Camera.GetProjectionMatrix();}

  auto GetRenderables() const
  {
    return m_Entities
        | std::views::values
        | std::views::filter([](const auto& entity)
        {
            return entity && entity->GetRenderable();
        })
        | std::views::transform([](const auto& entity)
        {
            const auto& renderable = *entity->GetRenderable();

            return RenderItem{
                .Renderable = renderable,
                .Transform =
                    entity->GetTransform() * renderable.GetTransform()
            };
        });
  }

  std::weak_ptr<Entity> GetEntity(const std::string& name) const;
protected:
  Camera& GetCamera(){return m_Camera;}

  std::string m_Name;
  //std::vector<std::unique_ptr<Entity>> m_Entities;
  std::unordered_map<std::string, std::shared_ptr<Entity>> m_Entities;
  Camera m_Camera;

  // Lights           container? <unique_ptr>
  // std::list<Light>

  ResourceManager& m_ResourceManager;

private:
  Window& m_window;

  void DrawFpsPanel(float deltaTime);
};

} // namespace Adachi