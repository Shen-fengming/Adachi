#pragma once

#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>

#include "RenderEnums.h"
#include "Buffer.h"
#include "UniformValue.h"
#include "Shader.h"
#include "Texture.h"


namespace Adachi{

struct MaterialDescCurrentPass
{
    std::shared_ptr<Shader> Shader;
    std::unordered_map<std::string, ShaderUniformType> ShaderUniformTypes;
    std::unordered_map<std::string, UniformValue> ShaderUniformValues;
    std::unordered_map<std::string, std::shared_ptr<Texture>> Textures;

    PrimitiveTopology Topology;
    DepthSetting DepthSetup;
    ColorSetting ColorSetting;
    AlphaSetting AlphaSetting;
};

class Material{
public:
    Material(const std::string& name): m_Name(name){}

    void AddShader(
        PassRenderTechnique technique,
        PrimitiveTopology topology,
        DepthSetting depthsetting,
        ColorSetting colorsetting,
        AlphaSetting alphasetting,
        std::shared_ptr<Shader> shader,
        std::unordered_map<std::string, ShaderUniformType> uniformtypes,
        std::unordered_map<std::string, UniformValue> uniformvalues = {},
        std::unordered_map<std::string, std::shared_ptr<Texture>> textures = {}
    );

    bool HasTechnique(PassRenderTechnique technique) const noexcept;
    Shader& GetShader(PassRenderTechnique technique) const;
    MaterialDescCurrentPass& GetCurrentPassDesc(PassRenderTechnique technique);
    const PrimitiveTopology& GetCurrentTopology(PassRenderTechnique technique) const;

    void SetUniformValue(PassRenderTechnique technique, const std::string& uniformName, const UniformValue& uniformValue);
    void SetColorWrite(bool red, bool green, bool blue, bool alpha);

private:
    std::string m_Name;
    std::unordered_map<PassRenderTechnique, MaterialDescCurrentPass> m_TechniqueShaderMap;


    // Cache: the state of current shader

};

}