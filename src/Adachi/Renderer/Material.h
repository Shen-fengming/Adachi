#pragma once

#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>

#include "Adachi/Diagnostics/Log.h"
#include "RenderEnums.h"
#include "Buffer.h"
#include "UniformValue.h"
#include "Shader.h"
#include "Adachi/Diagnostics/Debug.h"

namespace Adachi{

struct MaterialDescCurrentPass
{
    std::shared_ptr<Shader> Shader;
    std::unordered_map<std::string, ShaderUniformType> UniformTypes;
    std::unordered_map<std::string, UniformValue> Values;
    PrimitiveTopology Topology;
    DepthSetting DepthSetup;
    ColorSetting ColorSetting;
};

class Material{
public:
    Material(const std::string& name): m_Name(name){}

    void AddShader(
        PassRenderTechnique technique,
        PrimitiveTopology topology,
        DepthSetting depthsetting,
        ColorSetting colorsetting,
        std::shared_ptr<Shader> shader,
        std::unordered_map<std::string, ShaderUniformType> uniformtypes,
        std::unordered_map<std::string, UniformValue> uniformvalues = {}
    )
    {
        auto [iterator, inserted] = m_TechniqueShaderMap.try_emplace(
            technique,
            MaterialDescCurrentPass{
                std::move(shader),
                std::move(uniformtypes),
                std::move(uniformvalues),
                topology,
                depthsetting,
                colorsetting
            }
        );

        ADACHI_CORE_ASSERT(
            inserted, 
            "Material {} already has setup used for technique {}", 
            m_Name, 
            static_cast<std::underlying_type_t<PassRenderTechnique>>(technique)
        );
    }

    bool HasTechnique(PassRenderTechnique technique) const noexcept
    {
        return m_TechniqueShaderMap.contains(technique);
    }

    Shader& GetShader(PassRenderTechnique technique) const
    {
        return *m_TechniqueShaderMap.at(technique).Shader;
    }

    void SetColorWrite(bool red, bool green, bool blue, bool alpha)
    {
        for (auto& [name, desc] : m_TechniqueShaderMap)
        {
            desc.ColorSetting.red = red;
            desc.ColorSetting.green = green;
            desc.ColorSetting.blue = blue;
            desc.ColorSetting.alpha = alpha;

            ADACHI_CORE_INFO("New colorwrite set in material {}'s desc: {}, {}, {}, {}", m_Name, red, green, blue, alpha);
        }
    }

    const MaterialDescCurrentPass& GetCurrentPassDesc(PassRenderTechnique technique) const
    {
        return m_TechniqueShaderMap.at(technique);
    }

    const PrimitiveTopology& GetCurrentTopology(PassRenderTechnique technique) const
    {
        return m_TechniqueShaderMap.at(technique).Topology;
    }

private:
    std::string m_Name;
    std::unordered_map<PassRenderTechnique, MaterialDescCurrentPass> m_TechniqueShaderMap;


    // Cache: the state of current shader

};

}