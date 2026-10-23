#include "RenderPipeline.h"
#include "Adachi/Diagnostics/Debug.h"
#include "Adachi/Diagnostics/Log.h"
#include "Adachi/Renderer/Buffer.h"

#include "Adachi/Renderer/UniformValue.h"
#include "imgui.h"
#include <imgui_impl_glfw.h>
#include "imgui_impl_opengl3.h"

namespace Adachi {

#pragma region RenderEncoder
void RenderEncoder::SetUniformBuffer(const std::string& bufferName, const std::string& elementName, const UniformValue& elementValue)
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

void RenderEncoder::SetShaderUniform(const Renderable& renderable, const std::string& uniformName, const UniformValue& uniformValue)
{
    auto& shader = renderable.GetMaterial().GetShader(m_Technique);
    auto& typeBuffer = renderable.GetMaterial().GetCurrentPassDesc(m_Technique).UniformTypes;
    auto typeIt = typeBuffer.find(uniformName);

    if (typeIt == typeBuffer.end())
    {
        ADACHI_CORE_INFO("Uniform {} not found", uniformName);
        return;
    }

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
    
    m_Renderer.SetShaderUniform(shader, uniformName, uniformValue);
}

void RenderEncoder::BindMaterialOf(const Renderable& renderable)
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
        //ADACHI_CORE_INFO("Setting shader's uniform using material data:");
        PrintUniform("pipeline data", name, value);
        m_Renderer.SetShaderUniform(shader, name, value);
    }
}

void RenderEncoder::Draw(const Renderable& renderable)
{
    const auto& meshAsset = renderable.GetMeshAsset();

    if (!meshAsset)
        return;

    for (const auto& mesh : meshAsset->Meshes)
    {
        if (!mesh)
            continue;

        auto& vertexArray = mesh->GetVertexArray();
        auto& indexBuffer = mesh->GetIndexBuffer();

        m_Renderer.Draw(vertexArray, indexBuffer);
    }
}
#pragma endregion

#pragma region RenderPipeline
void RenderPipeline::Execute(Renderer& renderer, Scene& scene)
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

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    renderer.EndFrame();
}
#pragma endregion

}