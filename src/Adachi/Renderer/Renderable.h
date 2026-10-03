#pragma once

#include <memory>
#include <string>
#include <utility>
#include <glm/gtc/matrix_transform.hpp>

#include "Adachi/Renderer/Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "glm/fwd.hpp"

namespace Adachi {

struct Renderable {
    std::string Name;
    std::shared_ptr<Mesh> MeshAsset;
    std::shared_ptr<Material> MaterialAsset;

    // transform shapes from renderable's space to entity's space
    glm::mat4 Transform = glm::mat4(1.0f);
    bool m_Visible = true;
    
    const bool IsVisible() const{
        return m_Visible;
    }
    void SetVisible(bool visible)
    {
        m_Visible = visible;
    }


    Renderable(const std::string& name):Name(name)
    {}

    void SetMesh(std::shared_ptr<Mesh> mesh){MeshAsset = std::move(mesh);}
    void SetMaterial(std::shared_ptr<Material> material)
    {
        MaterialAsset = std::move(material);
    }

    bool ShouldRenderInPass(PassRenderTechnique technique) const 
    {
        return MaterialAsset &&
               MaterialAsset->HasTechnique(technique);
    }

    const Shader& GetShader(PassRenderTechnique technique)
    {
        return MaterialAsset->GetShader(technique);
    }

    const Material& GetMaterial () const
    {
        return *MaterialAsset;
    }

    static std::shared_ptr<Renderable>CreateRenderable(const std::string& name)
    {
        return std::make_shared<Renderable>(name);
    }
};

class Entity {
public:
    Entity(const std::string& name):m_Name(name)
    {
        CalculateTransform();
    }

    void SetRenderable(std::shared_ptr<Renderable> Renderable){m_Renderable = std::move(Renderable);}
    void SetTransform(const glm::mat4& transform){m_Transform = transform;}

    Adachi::Renderable* GetRenderable() const noexcept {
        return m_Renderable.get();
    }

    const glm::vec3& GetPosition() const {
        return m_Position;
    }

    const glm::mat4& GetTransform() const noexcept {
        return m_Transform;
    }

    void SetPosition(const glm::vec3& position)
    {
        m_Position = position;
        CalculateTransform();
    }

    void SetRotation(const glm::vec3& rotation)
    {
        m_Rotation = rotation;
        CalculateTransform();
    }

    void SetScale(const glm::vec3& scale)
    {
        m_Scale = scale;
        CalculateTransform();
    }

    static std::unique_ptr<Entity> Create(const std::string& name)
    {
        return std::make_unique<Entity>(name);
    }
private:
    void CalculateTransform()
    {
        glm::mat4 transform{1.0f};

        transform = glm::translate(transform, m_Position);
        transform *= glm::rotate(glm::mat4{1.0f}, m_Rotation.z, glm::vec3{0, 0, 1});
        transform *= glm::rotate(glm::mat4{1.0f}, m_Rotation.y, glm::vec3{0, 1, 0});
        transform *= glm::rotate(glm::mat4{1.0f}, m_Rotation.x, glm::vec3{1, 0, 0});
        transform = glm::scale(transform, m_Scale);

        m_Transform = transform;
    }

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