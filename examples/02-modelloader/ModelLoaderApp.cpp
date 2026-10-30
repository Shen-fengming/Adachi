// Entry Point: src/Adachi/Application/EntryPoint.cpp

#include "Adachi/Core/Application.h"
#include "Adachi/Scene/Scene.h"
#include "ModelLoaderScene.h"
#include <memory>

class ModelLoaderApp: public Adachi::Application{
public:
    ModelLoaderApp(const Adachi::Application::Config &config):Application(config){
        m_Scene = CreateScene<ModelLoaderScene>();
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

                //renderEncoder.SetUniformBuffer("LightAndColor", "u_Color", glm::vec3(0.3f, 0.7f, 1.0f));
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
    ~ModelLoaderApp(){
        m_Scene->OnDetach();
    };
};

Adachi::Application::Config Adachi::CreateApplicationConfig(Adachi::Application::CommandLineArg commandLineArg){
    Adachi::Application::Config config;
    config.Name = "02-ModelLoader";
    config.WorkingDirectory = "examples/02-modelloader";
    config.CommandLineArg = commandLineArg;

    return config;
}

Adachi::Application* Adachi::CreateApplication(Adachi::Application::Config config){
    return new ModelLoaderApp(config);
}