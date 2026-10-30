#include "GrassScene.h"
#include "Adachi/Renderer/Buffer.h"
#include "Adachi/Renderer/RenderEnums.h"
#include "Adachi/Renderer/Texture.h"
#include "Adachi/Renderer/UniformValue.h"
#include "Adachi/Resource/ResourceLoader.h"
#include "Adachi/Resource/ResourceManager.h"
#include "Adachi/Scene/Scene.h"

#include <imgui.h>
#include <GLFW/glfw3.h>

GrassScene::GrassScene(Adachi::ResourceManager& resourceManager, Adachi::Window& window):Adachi::Scene("02-ModelLoader", resourceManager, window)
{
    auto m_Shader_Bunny = m_ResourceManager.FetchShader("grass.vs", "grass.fs");
    auto m_Texture_Grass = m_ResourceManager.FetchTexture("grass.psd", Adachi::TextureSemantic::BaseColor, Adachi::TextureDesc{});
    auto m_Material_Grass = std::make_shared<Adachi::Material>("Grass Material");
    m_Material_Grass->AddShader(
		Adachi::PassRenderTechnique::Geometry,
		Adachi::PrimitiveTopology::TriangleList,
		{
			true,
			true,
			Adachi::CompareOperation::Less
		},
		{
			true,
			true,
			true,
			true
		},
        {
            true,
            0.5
        },
		m_Shader_Bunny, 
		{
			{"u_Model", Adachi::ShaderUniformType::Mat4},
			{"u_Color", Adachi::ShaderUniformType::Float3},
            {"u_AlphaClipping", Adachi::ShaderUniformType::Bool},
            {"u_AlphaCutoff", Adachi::ShaderUniformType::Float},
		},
		{
			{"u_Color", m_GrassColor},
            {"u_AlphaClipping", true},
            {"u_AlphaCutoff", 0.5f},
		},
        {
            {"texture1", std::move(m_Texture_Grass)}
        }
	);

    auto m_MeshAsset_Grass = m_ResourceManager.FetchFromModel("grass.fbx", Adachi::ModelPart::Meshes).meshAsset;
    auto m_Renderable_Grass = Adachi::Renderable::CreateRenderable("Grass Renderable", std::move(m_MeshAsset_Grass), std::move(m_Material_Grass));
    auto m_Entity_Grass = Adachi::Entity::Create("Grass");
    m_Entity_Grass->SetPosition(m_GrassPosition);
    m_Entity_Grass->SetRenderable(std::move(m_Renderable_Grass));
    m_Entities.emplace("Grass",std::move(m_Entity_Grass));
}

void GrassScene::OnAttach()
{
    Adachi::Scene::OnAttach();
}

void GrassScene::OnDetach()
{
	Adachi::Scene::OnDetach();
}

void GrassScene::OnUpdate(float time)
{
	Adachi::Scene::OnUpdate(time);

	auto bunny = GetEntity("Grass");

	ImGui::Begin("Settings");

	if (auto entity = bunny.lock())
	{
		if(ImGui::DragFloat3("Position", &m_GrassPosition.x, 0.05f))
		{
			entity->SetPosition(m_GrassPosition);
		}

		if(ImGui::DragFloat3("Rotation", &m_GrassRotation.x, 0.5f))
		{
			entity->SetRotation(m_GrassRotation);
		}

		if(ImGui::DragFloat3("Scale", &m_GrassScale.x, 0.01f))
		{
			entity->SetScale(m_GrassScale);
		}

		if (ImGui::ColorEdit3("Color", &m_GrassColor.x))
        {
			Adachi::PrintUniform("new color", "u_Color", m_GrassColor);
			
			auto& material = m_Entities.find("Grass")->second->GetRenderable()->GetMaterial();
            // Get a mutable reference to the bunny's material here.
            material.SetUniformValue(
                Adachi::PassRenderTechnique::Geometry,
                "u_Color",
                m_GrassColor
            );
        }
	}

    ImGui::End();
}
