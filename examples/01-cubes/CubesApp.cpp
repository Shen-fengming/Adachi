// Entry Point: src/Adachi/Application/EntryPoint.cpp

#include "Adachi/Core/Application.h"
#include "Adachi/Core/Scene.h"
#include "Adachi/Renderer/Framebuffer.h"
#include "Adachi/Renderer/RenderPipeline.h"
#include "CubesScene.h"
#include <memory>



class CubesApp: public Adachi::Application{
public:
    CubesApp(const Adachi::Application::Config &config):Application(config){
        m_Scene = std::make_unique<CubeScene>();
        m_Scene->imgui_window = m_Window.get();
        m_Scene->OnAttach();

        std::shared_ptr<Adachi::Framebuffer> m_Framebuffer;
        m_Framebuffer = Adachi::Framebuffer::CreateDefault();

        auto UniformbufferLayout = Adachi::UniformBufferLayout
        (
            {
                {"projection", 	Adachi::UniformBufferDataType::Mat4},
                {"view",		Adachi::UniformBufferDataType::Mat4}
            }
        );
        std::unique_ptr<Adachi::UniformBuffer> m_Uniformbuffer_view = Adachi::UniformBuffer::CreateUniformBuffer(UniformbufferLayout, 0);

        m_Pipeline->AddUniformBuffer("Matrices", std::move(m_Uniformbuffer_view));

        Adachi::RenderPass renderPass {
            .Description = {
                .Name = "CubeGeometry",

                .Technique = Adachi::PassRenderTechnique::Geometry,

                .Viewport = { 0.0f, 0.0f, 1.0f, 1.0f },

                .Framebuffer = std::move(m_Framebuffer),

                .ColorAttachments = {
                    {
                        .loadOperation = Adachi::LoadOperation::Clear,
                        .clearColor = { 0.05f, 0.05f, 0.08f, 1.0f }
                    }
                }
            },

            .Prepare = [](Adachi::RenderEncoder& renderEncoder, const Adachi::Scene& scene)
            {
                // Global Uniform update
                renderEncoder.SetUniformBuffer("Matrices", "projection", scene.GetProjectionMat());
                renderEncoder.SetUniformBuffer("Matrices", "view", scene.GetViewMat());
            },

            .Accepts = [](const Adachi::Renderable& renderable)
            {
                return renderable.IsVisible()&&renderable.ShouldRenderInPass(Adachi::PassRenderTechnique::Geometry);
            },

            .Draw = [](Adachi::RenderEncoder& renderEncoder, const Adachi::Renderable& renderable, const glm::mat4& transform, const Adachi::Scene& scene)
            {
                renderEncoder.BindMaterialOf(renderable);
                renderEncoder.SetShaderUniform(renderable, "u_model", transform);
                renderEncoder.Draw(renderable);
            },
        };

        m_Pipeline->AddPass(std::move(renderPass));

    };
    ~CubesApp(){
        m_Scene->OnDetach();
    };
};

Adachi::Application::Config Adachi::CreateApplicationConfig(Adachi::Application::CommandLineArg commandLineArg){
    Adachi::Application::Config config;
    config.Name = "01-Cubes";
    config.WorkingDirectory = "examples/01-cubes";
    config.CommandLineArg = commandLineArg;

    return config;
}

Adachi::Application* Adachi::CreateApplication(Adachi::Application::Config config){
    return new CubesApp(config);
}