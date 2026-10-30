#include "ModelLoaderScene.h"
#include "Adachi/Renderer/Buffer.h"
#include "Adachi/Renderer/RenderEnums.h"
#include "Adachi/Renderer/UniformValue.h"
#include "Adachi/Resource/ResourceLoader.h"
#include "Adachi/Resource/ResourceManager.h"
#include "Adachi/Scene/Scene.h"

#include <imgui.h>
#include <GLFW/glfw3.h>

ModelLoaderScene::ModelLoaderScene(Adachi::ResourceManager& resourceManager, Adachi::Window& window):Adachi::Scene("02-ModelLoader", resourceManager, window)
{
    auto m_MeshAsset_Bunny = m_ResourceManager.FetchFromModel("bunny.obj", Adachi::ModelPart::Meshes).meshAsset;

    auto m_Shader_Bunny = m_ResourceManager.FetchShader("model_simple.vs", "model_simple.fs");
    auto m_Material_Bunny = std::make_shared<Adachi::Material>("Bunny Material");
    m_Material_Bunny->AddShader(
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
            false,
            0.5
        },
		m_Shader_Bunny, 
		{
			{"u_Model", Adachi::ShaderUniformType::Mat4},
			{"u_Color", Adachi::ShaderUniformType::Float3}
			//texture
		},
		{
			{"u_Color", m_BunnyColor}
		}
	);

    auto m_Renderable_Bunny = Adachi::Renderable::CreateRenderable("Bunny Renderable", std::move(m_MeshAsset_Bunny), std::move(m_Material_Bunny));

    auto m_Entity_Bunny = Adachi::Entity::Create("Bunny");
    m_Entity_Bunny->SetPosition(glm::vec3(0.0f, 0.0f, -3.0f));
    m_Entity_Bunny->SetRenderable(std::move(m_Renderable_Bunny));
    m_Entities.emplace("Bunny",std::move(m_Entity_Bunny));
}

void ModelLoaderScene::OnAttach()
{
    Adachi::Scene::OnAttach();
}

void ModelLoaderScene::OnDetach()
{
	Adachi::Scene::OnDetach();
}

void ModelLoaderScene::OnUpdate(float time)
{
	Adachi::Scene::OnUpdate(time);

	auto bunny = GetEntity("Bunny");

	ImGui::Begin("Settings");

	if (auto entity = bunny.lock())
	{
		if(ImGui::DragFloat3("Position", &m_BunnyPosition.x, 0.05f))
		{
			entity->SetPosition(m_BunnyPosition);
		}

		if(ImGui::DragFloat3("Rotation", &m_BunnyRotation.x, 0.5f))
		{
			entity->SetRotation(m_BunnyRotation);
		}

		if(ImGui::DragFloat3("Scale", &m_BunnyScale.x, 0.01f))
		{
			entity->SetScale(m_BunnyScale);
		}

		if (ImGui::ColorEdit3("Color", &m_BunnyColor.x))
        {
			Adachi::PrintUniform("new color", "u_Color", m_BunnyColor);
			
			auto& material = m_Entities.find("Bunny")->second->GetRenderable()->GetMaterial();
            // Get a mutable reference to the bunny's material here.
            material.SetUniformValue(
                Adachi::PassRenderTechnique::Geometry,
                "u_Color",
                m_BunnyColor
            );
        }
	}

    ImGui::End();
}
