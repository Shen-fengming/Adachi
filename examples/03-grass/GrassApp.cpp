// Entry Point: src/Adachi/Application/EntryPoint.cpp

#include "Adachi/Core/Application.h"
#include "Adachi/Scene/Scene.h"
#include "GrassScene.h"
#include <memory>

class GrassApp: public Adachi::Application{
public:
    GrassApp(const Adachi::Application::Config &config):Application(config){
        m_Scene = CreateScene<GrassScene>();
        m_Scene->OnAttach();

        std::shared_ptr<Adachi::Framebuffer> m_Framebuffer = Adachi::Framebuffer::CreateDefault();

        auto UniformbufferLayout_0 = Adachi::UniformBufferLayout
        (
            {
                {"projection", 	Adachi::UniformBufferDataType::Mat4},
                {"view",		Adachi::UniformBufferDataType::Mat4}
            }
        );
        std::unique_ptr<Adachi::UniformBuffer> m_Uniformbuffer_view_0 = Adachi::UniformBuffer::Create(UniformbufferLayout_0, 0);
        m_Pipeline->AddUniformBuffer("Matrices", std::move(m_Uniformbuffer_view_0));

        auto UniformbufferLayout_1 = Adachi::UniformBufferLayout
        (
            {
                //{"u_Color", 	Adachi::UniformBufferDataType::Float3},
                {"u_LightDirection",		Adachi::UniformBufferDataType::Float3}
            }
        );
        std::unique_ptr<Adachi::UniformBuffer> m_Uniformbuffer_view_1 = Adachi::UniformBuffer::Create(UniformbufferLayout_1, 1);
        m_Pipeline->AddUniformBuffer("Light", std::move(m_Uniformbuffer_view_1));

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
                renderEncoder.SetUniformBuffer("Matrices", "projection", scene.GetProjectionMat());
                renderEncoder.SetUniformBuffer("Matrices", "view", scene.GetViewMat());

                renderEncoder.SetUniformBuffer("Light", "u_LightDirection", glm::vec3(-0.5f, -1.0f, -0.3f ));
            },

            .Accepts = [](const Adachi::Renderable& renderable)
            {
                return renderable.IsVisible()&&renderable.ShouldRenderInPass(Adachi::PassRenderTechnique::Geometry);
            },

            .Draw = [](Adachi::RenderEncoder& renderEncoder, const Adachi::Renderable& renderable, const glm::mat4& transform, const Adachi::Scene& scene)
            {
                renderEncoder.BindMaterialOf(renderable);
                renderEncoder.SetShaderUniform(renderable, "u_Model", transform);
                renderEncoder.Draw(renderable);
            },
        };

        m_Pipeline->AddPass(std::move(renderPass));
    };
    ~GrassApp(){
        m_Scene->OnDetach();
    };
};

Adachi::Application::Config Adachi::CreateApplicationConfig(Adachi::Application::CommandLineArg commandLineArg){
    Adachi::Application::Config config;
    config.Name = "03-Grass";
    config.WorkingDirectory = "examples/03-grass";
    config.CommandLineArg = commandLineArg;

    return config;
}

Adachi::Application* Adachi::CreateApplication(Adachi::Application::Config config){
    return new GrassApp(config);
}