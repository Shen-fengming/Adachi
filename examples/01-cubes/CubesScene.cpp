#include <cstdint>
#include <memory>
#include<string>
#include <glm/glm.hpp>
#include <utility>

#include "CubesScene.h"
#include "Adachi/Renderer/Mesh.h"
#include "Adachi/Scene/Scene.h"
#include "Adachi/Renderer/Buffer.h"
#include "Adachi/Renderer/RenderEnums.h"
#include "Adachi/Renderer/Renderable.h"
#include "Adachi/Renderer/VertexArray.h"
#include "Adachi/Diagnostics/Debug.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

float s_cubeVertices[8 * 7] = {
	-1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	 1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f
};

static const uint16_t s_cubeTriList[] =
{
	0, 1, 2, // 0
	1, 3, 2,
	4, 6, 5, // 2
	5, 6, 7,
	0, 2, 4, // 4
	4, 2, 6,
	1, 5, 3, // 6
	5, 7, 3,
	0, 4, 1, // 8
	4, 5, 1,
	2, 3, 6, // 10
	6, 3, 7,
};

static const uint16_t s_cubeTriStrip[] =
{
	0, 1, 2,
	3,
	7,
	1,
	5,
	0,
	4,
	2,
	6,
	7,
	4,
	5,
};

static const uint16_t s_cubeLineList[] =
{
	0, 1,
	0, 2,
	0, 4,
	1, 3,
	1, 5,
	2, 3,
	2, 6,
	3, 7,
	4, 5,
	4, 6,
	5, 7,
	6, 7,
};

static const uint16_t s_cubeLineStrip[] =
{
	0, 2, 3, 1, 5, 7, 6, 4,
	0, 2, 6, 4, 5, 7, 3, 1,
	0,
};

static const uint16_t s_cubePoints[] =
{
	0, 1, 2, 3, 4, 5, 6, 7
};

static const char* topologyNames[] = {
	"Triangle List",
	"Triangle Strip",
	"Line List",
	"Line Strip",
	"Points"
};

static Adachi::PrimitiveTopology topologyValues[] = {
        Adachi::PrimitiveTopology::TriangleList,
        Adachi::PrimitiveTopology::TriangleStrip,
		Adachi::PrimitiveTopology::LineList,
		Adachi::PrimitiveTopology::LineStrip,
		Adachi::PrimitiveTopology::PointList
};

CubeScene::CubeScene(Adachi::ResourceManager& resourceManager, Adachi::Window& window):Adachi::Scene("01-Cubes", resourceManager, window){
    
    auto bufferLayout_cube = Adachi::BufferLayout
	(
		{
        {"a_Position",  Adachi::ShaderUniformType::Float3},
        {"a_Color",     Adachi::ShaderUniformType::Float4}
    	}
	);
	auto m_VB = Adachi::VertexBuffer::Create(s_cubeVertices, sizeof(s_cubeVertices),std::move(bufferLayout_cube));
    
	//auto m_Shader = Adachi::Shader::Create("cubes_old", "cubes.vs", "cubes.fs");
	auto m_Shader = m_ResourceManager.FetchShader("cubes.vs", "cubes.fs");

	//auto m_Shader = m_Resources.LoadShader("cubes.vs", "cubes.fs")
#pragma region Triangle List
	// auto m_IB_TL = Adachi::IndexBuffer::Create(s_cubeTriList, sizeof(s_cubeTriList));
	auto m_IB_TL = Adachi::IndexBuffer::Create(s_cubeTriList, sizeof(s_cubeTriList), Adachi::IndexType::UInt16);

	auto m_VA_TL = Adachi::VertexArray::Create();
	m_VA_TL->AddVertexBuffer(m_VB);
	//m_VA_TL->SetIndexBuffer(m_IB_TL);

	auto m_Mesh_TL = Adachi::Mesh::Create("Cube Mesh TL", m_VA_TL, m_IB_TL);
	auto m_MeshAsset_TL = std::make_shared<Adachi::MeshAsset>();
	m_MeshAsset_TL->Meshes.emplace_back(std::move(m_Mesh_TL));

	auto m_Material_TL = std::make_shared<Adachi::Material>("Cube Material TL");
	m_Material_TL->AddShader(
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
		m_Shader, 
		{
			{"u_model", Adachi::ShaderUniformType::Mat4},
		}
	);
	m_Materials.push_back(m_Material_TL);

	auto m_Renderable_TL = Adachi::Renderable::CreateRenderable("Cube Renderable TL", std::move(m_MeshAsset_TL), std::move(m_Material_TL));

	for(uint32_t yy=0;yy<11;yy++)
	{
		for(uint32_t xx=0;xx<11;xx++)
		{
			std::string name = "Cube_TL_" + std::to_string(11*yy+xx);
			auto m_Entity = Adachi::Entity::Create(std::move(name));
			m_Entity->SetPosition(glm::vec3(-15.0f+float(xx)*3.0f, -15.0f+float(yy)*3.0f, -50.0f));
			m_Entity->SetRenderable(m_Renderable_TL);
			m_Entities.emplace(m_Entity->GetName(),std::move(m_Entity));
		}
	}

#pragma endregion

#pragma region Triangle Strip
	auto m_IB_TS = Adachi::IndexBuffer::Create(s_cubeTriStrip, sizeof(s_cubeTriStrip),Adachi::IndexType::UInt16);

	auto m_VA_TS = Adachi::VertexArray::Create();
	m_VA_TS->AddVertexBuffer(m_VB);
	//m_VA_TS->SetIndexBuffer(m_IB_TS);

	auto m_Mesh_TS = Adachi::Mesh::Create("Cube Mesh TS", m_VA_TS, m_IB_TS);
	auto m_MeshAsset_TS = std::make_shared<Adachi::MeshAsset>();
	m_MeshAsset_TS->Meshes.emplace_back(std::move(m_Mesh_TS));

	auto m_Material_TS = std::make_shared<Adachi::Material>("Cube Material TS");
	m_Material_TS->AddShader(
		Adachi::PassRenderTechnique::Geometry,
		Adachi::PrimitiveTopology::TriangleStrip,
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
		m_Shader, 
		{
			{"u_model", Adachi::ShaderUniformType::Mat4},
		}
	);
	m_Materials.push_back(m_Material_TS);

	auto m_Renderable_TS = Adachi::Renderable::CreateRenderable("Cube Renderable TS",std::move(m_MeshAsset_TS), std::move(m_Material_TS));

	for(uint32_t yy=0;yy<11;yy++)
	{
		for(uint32_t xx=0;xx<11;xx++)
		{
			std::string name = "Cube_TS_" + std::to_string(11*yy+xx);
			auto m_Entity = Adachi::Entity::Create(std::move(name));
			m_Entity->SetPosition(glm::vec3(-15.0f+float(xx)*3.0f, -15.0f+float(yy)*3.0f, -50.0f));
			m_Entity->SetRenderable(m_Renderable_TS);
			m_Entities.emplace(m_Entity->GetName(),std::move(m_Entity));
		}
	}
#pragma endregion

#pragma region Line List
	auto m_IB_LL = Adachi::IndexBuffer::Create(s_cubeLineList, sizeof(s_cubeLineList),Adachi::IndexType::UInt16);

	auto m_VA_LL = Adachi::VertexArray::Create();
	m_VA_LL->AddVertexBuffer(m_VB);

	auto m_Mesh_LL = Adachi::Mesh::Create("Cube Mesh LL", m_VA_LL, m_IB_LL);
	auto m_MeshAsset_LL = std::make_shared<Adachi::MeshAsset>();
	m_MeshAsset_LL->Meshes.emplace_back(std::move(m_Mesh_LL));

	auto m_Material_LL = std::make_shared<Adachi::Material>("Cube Material LL");
	m_Material_LL->AddShader(
		Adachi::PassRenderTechnique::Geometry,
		Adachi::PrimitiveTopology::LineList,
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
		m_Shader, 
		{
			{"u_model", Adachi::ShaderUniformType::Mat4},
		}
	);
	m_Materials.push_back(m_Material_LL);

	auto m_Renderable_LL = Adachi::Renderable::CreateRenderable("Cube Renderable LL", std::move(m_MeshAsset_LL), std::move(m_Material_LL));

	for(uint32_t yy=0;yy<11;yy++)
	{
		for(uint32_t xx=0;xx<11;xx++)
		{
			std::string name = "Cube_LL_" + std::to_string(11*yy+xx);
			auto m_Entity = Adachi::Entity::Create(std::move(name));
			m_Entity->SetPosition(glm::vec3(-15.0f+float(xx)*3.0f, -15.0f+float(yy)*3.0f, -50.0f));
			m_Entity->SetRenderable(m_Renderable_LL);
			m_Entities.emplace(m_Entity->GetName(),std::move(m_Entity));
		}
	}
#pragma endregion

#pragma region Line Strip
	auto m_IB_LS = Adachi::IndexBuffer::Create(s_cubeLineStrip, sizeof(s_cubeLineStrip),Adachi::IndexType::UInt16);

	auto m_VA_LS = Adachi::VertexArray::Create();
	m_VA_LS->AddVertexBuffer(m_VB);

	auto m_Mesh_LS = Adachi::Mesh::Create("Cube Mesh LS", m_VA_LS, m_IB_LS);
	auto m_MeshAsset_LS = std::make_shared<Adachi::MeshAsset>();
	m_MeshAsset_LS->Meshes.emplace_back(std::move(m_Mesh_LS));

	auto m_Material_LS = std::make_shared<Adachi::Material>("Cube Material LS");
	m_Material_LS->AddShader(
		Adachi::PassRenderTechnique::Geometry,
		Adachi::PrimitiveTopology::LineStrip,
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
		m_Shader, 
		{
			{"u_model", Adachi::ShaderUniformType::Mat4},
		}
	);
	m_Materials.push_back(m_Material_LS);

	auto m_Renderable_LS = Adachi::Renderable::CreateRenderable("Cube Renderable LS", std::move(m_MeshAsset_LS), std::move(m_Material_LS));

	for(uint32_t yy=0;yy<11;yy++)
	{
		for(uint32_t xx=0;xx<11;xx++)
		{
			std::string name = "Cube_LS_" + std::to_string(11*yy+xx);
			auto m_Entity = Adachi::Entity::Create(std::move(name));
			m_Entity->SetPosition(glm::vec3(-15.0f+float(xx)*3.0f, -15.0f+float(yy)*3.0f, -50.0f));
			m_Entity->SetRenderable(m_Renderable_LS);
			m_Entities.emplace(m_Entity->GetName(),std::move(m_Entity));
		}
	}
#pragma endregion

#pragma region Points
	auto m_IB_P = Adachi::IndexBuffer::Create(s_cubePoints, sizeof(s_cubePoints),Adachi::IndexType::UInt16);

	auto m_VA_P = Adachi::VertexArray::Create();
	m_VA_P->AddVertexBuffer(m_VB);

	auto m_Mesh_P = Adachi::Mesh::Create("Cube Mesh LS", m_VA_P, m_IB_P);
	auto m_MeshAsset_P = std::make_shared<Adachi::MeshAsset>();
	m_MeshAsset_P->Meshes.emplace_back(std::move(m_Mesh_P));

	auto m_Material_P = std::make_shared<Adachi::Material>("Cube Material P");
	m_Material_P->AddShader(
		Adachi::PassRenderTechnique::Geometry,
		Adachi::PrimitiveTopology::PointList,
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
		m_Shader, 
		{
			{"u_model", Adachi::ShaderUniformType::Mat4},
		}
	);
	m_Materials.push_back(m_Material_P);
	
	auto m_Renderable_P = Adachi::Renderable::CreateRenderable("Cube Renderable P", std::move(m_MeshAsset_P), std::move(m_Material_P));

	for(uint32_t yy=0;yy<11;yy++)
	{
		for(uint32_t xx=0;xx<11;xx++)
		{
			std::string name = "Cube_P_" + std::to_string(11*yy+xx);
			auto m_Entity = Adachi::Entity::Create(std::move(name));
			m_Entity->SetPosition(glm::vec3(-15.0f+float(xx)*3.0f, -15.0f+float(yy)*3.0f, -50.0f));
			m_Entity->SetRenderable(m_Renderable_P);
			m_Entities.emplace(m_Entity->GetName(),std::move(m_Entity));
		}
	}
#pragma endregion

	for(auto& [name, entity]: m_Entities)
	{
		if(entity->GetRenderable()->GetMaterial().GetCurrentTopology(Adachi::PassRenderTechnique::Geometry) == m_currentTopology)
		{
			entity->GetRenderable()->SetVisible(true);
		}
		else 
		{
			entity->GetRenderable()->SetVisible(false);
		}
	}

	m_Renderable_TL.reset();
	m_Shader.reset();
	m_Mesh_TL.reset();
	m_VA_TL.reset();

	ADACHI_APP_ASSERT((!m_VA_TL)&&(!m_Shader)&&(!m_Material_TL)&&(!m_Mesh_TL), "It should be empty");
}

void CubeScene::OnAttach()
{
	Adachi::Scene::OnAttach();
}

void CubeScene::OnDetach()
{
	Adachi::Scene::OnDetach();
}

void CubeScene::OnUpdate(float time)
{
	Adachi::Scene::OnUpdate(time);

	ImGui::Begin("Settings");
	// setup about color passes
	m_colorWriteChanged |= ImGui::Checkbox("Red",   &m_R);
	m_colorWriteChanged |= ImGui::Checkbox("Green", &m_G);
	m_colorWriteChanged |= ImGui::Checkbox("Blue",  &m_B);
	m_colorWriteChanged |= ImGui::Checkbox("Alpha", &m_A);

	ImGui::Text("Primitive topology:");
	if (ImGui::Combo(
            "##PrimitiveTopology",
            &selectedIndex,
            topologyNames,
            static_cast<int>(std::size(topologyNames))))
    {
        m_currentTopology = topologyValues[selectedIndex];
        m_topologyChanged = true;
    }
	ImGui::End();

	if(m_topologyChanged)
	{
		for(auto& [name, entity]: m_Entities)
		{
			if(entity->GetRenderable()->GetMaterial().GetCurrentTopology(Adachi::PassRenderTechnique::Geometry) == m_currentTopology)
			{
				entity->GetRenderable()->SetVisible(true);
			}
			else 
			{
				entity->GetRenderable()->SetVisible(false);
			}
		}

		m_topologyChanged = false;
	}

	// Change Material property
	if(m_colorWriteChanged)
	{
		for(auto& material: m_Materials)
		{
			material->SetColorWrite(m_R, m_G, m_B, m_A);
		}

		m_colorWriteChanged = false;
	}

}