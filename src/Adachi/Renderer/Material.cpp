#include "Material.h"
#include "Adachi/Diagnostics/Debug.h"

namespace Adachi
{
void Material::AddShader(
    PassRenderTechnique technique,
    PrimitiveTopology topology,
    DepthSetting depthsetting,
    ColorSetting colorsetting,
    std::shared_ptr<Shader> shader,
    std::unordered_map<std::string, ShaderUniformType> uniformtypes,
    std::unordered_map<std::string, UniformValue> uniformvalues
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

bool Material::HasTechnique(PassRenderTechnique technique) const noexcept
{
    return m_TechniqueShaderMap.contains(technique);
}

Shader& Material::GetShader(PassRenderTechnique technique) const
{
    return *m_TechniqueShaderMap.at(technique).Shader;
}

MaterialDescCurrentPass& Material::GetCurrentPassDesc(PassRenderTechnique technique)
{
    return m_TechniqueShaderMap.at(technique);
}

const PrimitiveTopology& Material::GetCurrentTopology(PassRenderTechnique technique) const
{
    return m_TechniqueShaderMap.at(technique).Topology;
}

void Material::SetUniformValue(PassRenderTechnique technique, const std::string& uniformName, const UniformValue& uniformValue)
{
    auto typeBuffer = GetCurrentPassDesc(technique).UniformTypes;
    auto typeIt = typeBuffer.find(uniformName);

    if (typeIt == typeBuffer.end())
    {
        ADACHI_CORE_INFO("Uniform {} not found", uniformName);
        return;
    }

    // size of uniformValue and the uniform type stored in material match
    std::visit(
        [&](const auto& value)
        {
            auto requiredSize = ShaderDataTypeSize(typeIt->second);
            auto receivedSize = sizeof(std::remove_cvref_t<decltype(value)>);

            if(requiredSize != receivedSize)
            {
                ADACHI_CORE_INFO(
                    "Value size does not match: Shader uniform {} requires size {}, receives size {}.",
                    uniformName,
                    requiredSize,
                    receivedSize
                );
            }
        },
        uniformValue
    );

    auto& valueBuffer = GetCurrentPassDesc(technique).Values;
    PrintUniform("new material color", "u_Color", uniformValue);
    valueBuffer.insert_or_assign(uniformName, uniformValue);
}

void Material::SetColorWrite(bool red, bool green, bool blue, bool alpha)
{
    for (auto& [name, desc] : m_TechniqueShaderMap)
    {
        desc.ColorSetting.red = red;
        desc.ColorSetting.green = green;
        desc.ColorSetting.blue = blue;
        desc.ColorSetting.alpha = alpha;
    }
}

}