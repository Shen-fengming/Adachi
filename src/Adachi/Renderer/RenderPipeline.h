#pragma once

#include <memory>
#include <string>

#include "Adachi/Core/Scene.h"
#include "Adachi/Diagnostics/Debug.h"
#include "Adachi/Diagnostics/Log.h"
#include "Adachi/Renderer/Buffer.h"
#include "Adachi/Renderer/Renderable.h"
#include "Adachi/Renderer/Renderer.h"
#include "Adachi/Renderer/UniformBuffer.h"

#include "imgui.h"
#include <imgui_impl_glfw.h>
#include "imgui_impl_opengl3.h"

namespace Adachi {

class RenderEncoder{
public:
    RenderEncoder(Renderer& renderer, PassRenderTechnique technique, std::unordered_map<std::string, std::unique_ptr<UniformBuffer>>& buffers):
    m_Renderer(renderer), m_Technique(technique), m_UniformBuffers(buffers)
    {
    }
public:
    void SetUniformBuffer(const std::string& bufferName, const std::string& elementName, const UniformValue& elementValue)
    {
        // Find the buffer using bufferName
        auto bufferIt = m_UniformBuffers.find(bufferName);

        if (bufferIt == m_UniformBuffers.end())
        {
            ADACHI_CORE_ASSERT(false, "Uniform buffer {} not found", bufferName);
        }

        const auto& element = bufferIt->second->GetElementByName(elementName);
        if(!element)
        {
            ADACHI_CORE_ASSERT(false, "Uniform element {} of buffer {} not found", elementName, bufferName);
        }


        // If ok, do m_renderer.SetUniformBuffer(buffer, elementName, value)
        const bool valid = std::visit(
            [&](const auto& value)
            {
                return element->Size == sizeof(std::remove_cvref_t<decltype(value)>);
            },
            elementValue
        );

        ADACHI_CORE_ASSERT(
            valid,
            "Value size does not match UniformBuffer {} element {}.",
            bufferName,
            elementName
        );

        m_Renderer.SetUniformBuffer(*bufferIt->second, elementName, elementValue);
    }

    void SetShaderUniform(const Renderable& renderable, const std::string& uniformName, const UniformValue& uniformValue)
    {
        auto& shader = renderable.GetMaterial().GetShader(m_Technique);
        auto& typeBuffer = renderable.GetMaterial().GetCurrentPassDesc(m_Technique).UniformTypes;
        auto typeIt = typeBuffer.find(uniformName);

        if (typeIt == typeBuffer.end())
        {
            ADACHI_CORE_INFO("Uniform {} not found", uniformName);
            return;
        }

        //ADACHI_CORE_INFO("Setting uniform {}", uniformName);
        m_Renderer.SetShaderUniform(shader, uniformName, uniformValue);
    }

    void BindMaterialOf(const Renderable& renderable)
    {
        auto& desc = renderable.GetMaterial().GetCurrentPassDesc(m_Technique);

        auto& shader = *desc.Shader;
        auto& values = desc.Values;
        auto& topology = desc.Topology;
        auto& depthsetup = desc.DepthSetup;
        auto& colorsetup = desc.ColorSetting;

        m_Renderer.SetTopology(topology);

        m_Renderer.SetDepthBuffer(depthsetup);

        m_Renderer.SetColorMask(colorsetup);

        m_Renderer.BindShader(shader);

        for(const auto&[name, value]: values)
        {
            m_Renderer.SetShaderUniform(shader, name, value);
        }
    }

    void Draw(const Renderable& renderable)
    {
        //Provide the vertexArray to renderer
        auto& vertexArray = *renderable.MeshAsset->VertexArrayObject;
        auto& indexBuffer = *renderable.MeshAsset->IndexBufferObject;
        m_Renderer.Draw(vertexArray, indexBuffer);
    }
private:
    Renderer& m_Renderer;
    std::unordered_map<std::string, std::unique_ptr<UniformBuffer>>& m_UniformBuffers;
    PassRenderTechnique m_Technique;
};


struct RenderPass{
    RenderPassDesc Description;

    // Setting uniform buffers
    std::function<void(RenderEncoder&, const Scene&)> Prepare;

    std::function<bool(const Renderable&)> Accepts;

    std::function<void(RenderEncoder&, const Renderable&, const glm::mat4&, const Scene& scene)> Draw;

    // func everyobject()
};

class RenderPipeline{
public:
    RenderPipeline() = default;
    ~RenderPipeline() = default;

    void AddPass(RenderPass pass)
    {
        m_Passes.emplace_back(std::move(pass));
    }
    void Execute(Renderer& renderer, Scene& scene)
    {
        renderer.BeginFrame();

        for(RenderPass& pass: m_Passes)
        {
            //ADACHI_CORE_INFO("Pass executing");
            
            renderer.BeginPass(pass.Description);

            // Start taking outside function
            RenderEncoder encoder(renderer,pass.Description.Technique, m_UniformBuffers);

            if(pass.Prepare)
                pass.Prepare(encoder, scene);

            if(pass.Draw)
            {
                for (auto item : scene.GetRenderables())
                {
                    if (!pass.Accepts || pass.Accepts(item.Renderable))
                        {
                            pass.Draw(encoder, item.Renderable, item.Transform, scene); 
                        } 
                }
            }
        }

        renderer.SetupForImGUI();
        ImDrawData* drawData = ImGui::GetDrawData();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        renderer.EndFrame();
    }

    void AddUniformBuffer(const std::string& name, std::unique_ptr<UniformBuffer> uniformBuffer)
    {
        m_UniformBuffers[name] = std::move(uniformBuffer);
    }

    //Execute();

private:
    //<>Passes
    std::vector<RenderPass> m_Passes;

    //std::unordered_map<Uniform buffer name, unique_ptr uniformbuffer>
    std::unordered_map<std::string, std::unique_ptr<UniformBuffer>> m_UniformBuffers;
};

}