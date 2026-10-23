#pragma once

#include <memory>
#include <string>
#include <utility>
#include <glm/gtc/matrix_transform.hpp>

#include "Material.h"
#include "Mesh.h"
#include "glm/fwd.hpp"

namespace Adachi {

class Renderable {
private:
    std::string m_Name;
    std::shared_ptr<MeshAsset> m_MeshAsset;
    std::shared_ptr<Material> m_MaterialAsset;

    // transform shapes from renderable's space to entity's space
    glm::mat4 m_Transform = glm::mat4(1.0f);
    bool m_Visible = true;
public:
    Renderable(const std::string& name, std::shared_ptr<MeshAsset> meshAsset, std::shared_ptr<Material> materialAsset):m_Name(name), m_MeshAsset(std::move(meshAsset)), m_MaterialAsset(std::move(materialAsset))
    {}

    const bool IsVisible() const{ return m_Visible; }
    void SetVisible(bool visible) { m_Visible = visible; }

    bool ShouldRenderInPass(PassRenderTechnique technique) const 
    {
        return m_MaterialAsset && m_MaterialAsset->HasTechnique(technique);
    }

    const glm::mat4&                    GetTransform() const {return m_Transform;}
    const std::shared_ptr<MeshAsset>&   GetMeshAsset() const noexcept { return m_MeshAsset; }
    Material&                     GetMaterial () const { return *m_MaterialAsset; }

    static std::shared_ptr<Renderable>CreateRenderable(const std::string& name, std::shared_ptr<MeshAsset> meshAsset, std::shared_ptr<Material> materialAsset)
    {
        return std::make_shared<Renderable>(name, std::move(meshAsset), std::move(materialAsset));
    }
};

}