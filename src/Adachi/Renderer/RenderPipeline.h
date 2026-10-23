#pragma once

#include <memory>
#include <string>

#include "Adachi/Scene/Scene.h"
#include "Adachi/Renderer/Renderable.h"
#include "Adachi/Renderer/Renderer.h"
#include "Adachi/Renderer/UniformBuffer.h"

namespace Adachi {

class RenderEncoder{
public:
    RenderEncoder(Renderer& renderer, PassRenderTechnique technique, std::unordered_map<std::string, std::unique_ptr<UniformBuffer>>& buffers):
    m_Renderer(renderer), m_Technique(technique), m_UniformBuffers(buffers)
    {
    }
public:
    void SetUniformBuffer(const std::string& bufferName, const std::string& elementName, const UniformValue& elementValue);
    void SetShaderUniform(const Renderable& renderable, const std::string& uniformName, const UniformValue& uniformValue);
    void BindMaterialOf(const Renderable& renderable);

    void Draw(const Renderable& renderable);
    
private:
    Renderer& m_Renderer;
    std::unordered_map<std::string, std::unique_ptr<UniformBuffer>>& m_UniformBuffers;
    PassRenderTechnique m_Technique;
};


struct RenderPass{
    RenderPassDesc Description;

    std::function<void(RenderEncoder&, const Scene&)> Prepare;

    std::function<bool(const Renderable&)> Accepts;

    std::function<void(RenderEncoder&, const Renderable&, const glm::mat4&, const Scene& scene)> Draw;
};

class RenderPipeline{
public:
    RenderPipeline() = default;
    ~RenderPipeline() = default;

    void AddPass(RenderPass pass) 
    {
        m_Passes.emplace_back(std::move(pass));
    }
    
    void AddUniformBuffer(const std::string& name, std::unique_ptr<UniformBuffer> uniformBuffer)
    {
        m_UniformBuffers[name] = std::move(uniformBuffer);
    }

    void Execute(Renderer& renderer, Scene& scene);
private:
    std::vector<RenderPass> m_Passes;
    std::unordered_map<std::string, std::unique_ptr<UniformBuffer>> m_UniformBuffers;
};

}