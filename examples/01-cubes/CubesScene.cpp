#include <cstdint>
#include <memory>
#include<string>
#include <glm/glm.hpp>
#include <utility>

#include "CubesScene.h"
#include "Adachi/Core/Scene.h"
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


CubeScene::CubeScene():Adachi::Scene("01-Cubes"){
    auto m_VB = Adachi::VertexBuffer::Create(s_cubeVertices, sizeof(s_cubeVertices));
    auto bufferLayout_cube = Adachi::BufferLayout
	(
		{
        {"a_Position",  Adachi::ShaderUniformType::Float3},
        {"a_Color",     Adachi::ShaderUniformType::Float4}
    	}
	);
	m_VB->SetLayout(bufferLayout_cube);
    
	auto m_Shader = Adachi::Shader::Create("cubes", "cubes.vs", "cubes.fs");
#pragma region Triangle List
	// auto m_IB_TL = Adachi::IndexBuffer::Create(s_cubeTriList, sizeof(s_cubeTriList));
	auto m_IB_TL = Adachi::IndexBuffer::Create(s_cubeTriList, sizeof(s_cubeTriList));

	auto m_VA_TL = Adachi::VertexArray::Create();
	m_VA_TL->AddVertexBuffer(m_VB);
	//m_VA_TL->SetIndexBuffer(m_IB_TL);

	auto m_Mesh_TL = Adachi::Mesh::CreateMesh("Cube Mesh TL", m_VA_TL, m_IB_TL);

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
		m_Shader, 
		{
			{"u_model", Adachi::ShaderUniformType::Mat4},
		}
	);
	m_Materials.push_back(m_Material_TL);

	auto m_Renderable_TL = Adachi::Renderable::CreateRenderable("Cube Renderable TL");
	m_Renderable_TL->SetMesh(m_Mesh_TL);
	m_Renderable_TL->SetMaterial(std::move(m_Material_TL));

	for(uint32_t yy=0;yy<11;yy++)
	{
		for(uint32_t xx=0;xx<11;xx++)
		{
			std::string name = "Cube_TL_" + std::to_string(11*yy+xx);
			auto m_Entity = Adachi::Entity::Create(std::move(name));
			m_Entity->SetPosition(glm::vec3(-15.0f+float(xx)*3.0f, -15.0f+float(yy)*3.0f, -50.0f));
			m_Entity->SetRenderable(m_Renderable_TL);
			m_Entities.push_back(std::move(m_Entity));
		}
	}

#pragma endregion

#pragma region Triangle Strip
	auto m_IB_TS = Adachi::IndexBuffer::Create(s_cubeTriStrip, sizeof(s_cubeTriStrip));

	auto m_VA_TS = Adachi::VertexArray::Create();
	m_VA_TS->AddVertexBuffer(m_VB);
	//m_VA_TS->SetIndexBuffer(m_IB_TS);

	auto m_Mesh_TS = Adachi::Mesh::CreateMesh("Cube Mesh TS", m_VA_TS, m_IB_TS);

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
		m_Shader, 
		{
			{"u_model", Adachi::ShaderUniformType::Mat4},
		}
	);
	m_Materials.push_back(m_Material_TS);

	auto m_Renderable_TS = Adachi::Renderable::CreateRenderable("Cube Renderable TS");
	m_Renderable_TS->SetMesh(m_Mesh_TS);
	m_Renderable_TS->SetMaterial(std::move(m_Material_TS));

	for(uint32_t yy=0;yy<11;yy++)
	{
		for(uint32_t xx=0;xx<11;xx++)
		{
			std::string name = "Cube_TS_" + std::to_string(11*yy+xx);
			auto m_Entity = Adachi::Entity::Create(std::move(name));
			m_Entity->SetPosition(glm::vec3(-15.0f+float(xx)*3.0f, -15.0f+float(yy)*3.0f, -50.0f));
			m_Entity->SetRenderable(m_Renderable_TS);
			m_Entities.push_back(std::move(m_Entity));
		}
	}
#pragma endregion

#pragma region Line List
	auto m_IB_LL = Adachi::IndexBuffer::Create(s_cubeLineList, sizeof(s_cubeLineList));

	auto m_VA_LL = Adachi::VertexArray::Create();
	m_VA_LL->AddVertexBuffer(m_VB);

	auto m_Mesh_LL = Adachi::Mesh::CreateMesh("Cube Mesh LL", m_VA_LL, m_IB_LL);

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
		m_Shader, 
		{
			{"u_model", Adachi::ShaderUniformType::Mat4},
		}
	);
	m_Materials.push_back(m_Material_LL);

	auto m_Renderable_LL = Adachi::Renderable::CreateRenderable("Cube Renderable LL");
	m_Renderable_LL->SetMesh(m_Mesh_LL);
	m_Renderable_LL->SetMaterial(std::move(m_Material_LL));

	for(uint32_t yy=0;yy<11;yy++)
	{
		for(uint32_t xx=0;xx<11;xx++)
		{
			std::string name = "Cube_LL_" + std::to_string(11*yy+xx);
			auto m_Entity = Adachi::Entity::Create(std::move(name));
			m_Entity->SetPosition(glm::vec3(-15.0f+float(xx)*3.0f, -15.0f+float(yy)*3.0f, -50.0f));
			m_Entity->SetRenderable(m_Renderable_LL);
			m_Entities.push_back(std::move(m_Entity));
		}
	}
#pragma endregion

#pragma region Line Strip
	auto m_IB_LS = Adachi::IndexBuffer::Create(s_cubeLineStrip, sizeof(s_cubeLineStrip));

	auto m_VA_LS = Adachi::VertexArray::Create();
	m_VA_LS->AddVertexBuffer(m_VB);

	auto m_Mesh_LS = Adachi::Mesh::CreateMesh("Cube Mesh LS", m_VA_LS, m_IB_LS);

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
		m_Shader, 
		{
			{"u_model", Adachi::ShaderUniformType::Mat4},
		}
	);
	m_Materials.push_back(m_Material_LS);

	auto m_Renderable_LS = Adachi::Renderable::CreateRenderable("Cube Renderable LS");
	m_Renderable_LS->SetMesh(m_Mesh_LS);
	m_Renderable_LS->SetMaterial(std::move(m_Material_LS));

	for(uint32_t yy=0;yy<11;yy++)
	{
		for(uint32_t xx=0;xx<11;xx++)
		{
			std::string name = "Cube_LS_" + std::to_string(11*yy+xx);
			auto m_Entity = Adachi::Entity::Create(std::move(name));
			m_Entity->SetPosition(glm::vec3(-15.0f+float(xx)*3.0f, -15.0f+float(yy)*3.0f, -50.0f));
			m_Entity->SetRenderable(m_Renderable_LS);
			m_Entities.push_back(std::move(m_Entity));
		}
	}
#pragma endregion

#pragma region Points
	auto m_IB_P = Adachi::IndexBuffer::Create(s_cubePoints, sizeof(s_cubePoints));

	auto m_VA_P = Adachi::VertexArray::Create();
	m_VA_P->AddVertexBuffer(m_VB);

	auto m_Mesh_P = Adachi::Mesh::CreateMesh("Cube Mesh LS", m_VA_P, m_IB_P);

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
		m_Shader, 
		{
			{"u_model", Adachi::ShaderUniformType::Mat4},
		}
	);
	m_Materials.push_back(m_Material_P);
	
	auto m_Renderable_P = Adachi::Renderable::CreateRenderable("Cube Renderable P");
	m_Renderable_P->SetMesh(m_Mesh_P);
	m_Renderable_P->SetMaterial(std::move(m_Material_P));

	for(uint32_t yy=0;yy<11;yy++)
	{
		for(uint32_t xx=0;xx<11;xx++)
		{
			std::string name = "Cube_P_" + std::to_string(11*yy+xx);
			auto m_Entity = Adachi::Entity::Create(std::move(name));
			m_Entity->SetPosition(glm::vec3(-15.0f+float(xx)*3.0f, -15.0f+float(yy)*3.0f, -50.0f));
			m_Entity->SetRenderable(m_Renderable_P);
			m_Entities.push_back(std::move(m_Entity));
		}
	}
#pragma endregion

	for(auto& entity: m_Entities)
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
	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	float x_scale = 1.0f;
	float y_scale = 1.0f;

	glfwGetWindowContentScale(static_cast<GLFWwindow*>(imgui_window->GetNativeHandle()), &x_scale, &y_scale);

	float main_scale = x_scale;

	const char* glsl_version = "#version 410 core";
	ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(imgui_window->GetNativeHandle()), true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void CubeScene::OnDetach()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void CubeScene::OnUpdate(float time)
{
	//Imgui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

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

	ImGui::Render();

	// setup about frame&&app

	
	if(m_topologyChanged)
	{
		for(auto& entity: m_Entities)
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