#pragma once

#include <memory>
#include <string>
#include <utility>
#include <glm/gtc/matrix_transform.hpp>

#include "Adachi/Renderer/Renderable.h"
#include "glm/fwd.hpp"

namespace Adachi {

class Entity {
public:
    Entity(const std::string& name):m_Name(name)
    {
        CalculateTransform();
    }

    void SetRenderable(std::shared_ptr<Renderable> Renderable){m_Renderable = std::move(Renderable);}
    void SetTransform(const glm::mat4& transform){m_Transform = transform;}

    const std::string GetName() const noexcept {return m_Name;}
    Adachi::Renderable* GetRenderable() const noexcept { return m_Renderable.get(); }
    const glm::vec3& GetPosition() const { return m_Position;}
    const glm::mat4& GetTransform() const noexcept { return m_Transform;}

    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);
    void SetScale(const glm::vec3& scale);

    static std::unique_ptr<Entity> Create(const std::string& name)
    {
        return std::make_unique<Entity>(name);
    }
private:
    void CalculateTransform();
    
    glm::vec3 m_Position = glm::vec3(0.0f);
    glm::vec3 m_Rotation = glm::vec3(0.0f);
    glm::vec3 m_Scale = glm::vec3(1.0f);;

    // Transform shapes from entity's space to world space
    glm::mat4 m_Transform;
private:
    std::string m_Name;
    std::shared_ptr<Renderable> m_Renderable;
};

}