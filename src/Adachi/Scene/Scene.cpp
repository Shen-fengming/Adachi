#include "Scene.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

namespace Adachi
{
void Scene::OnAttach()
{
    ImGui::CreateContext();
	ImGui::StyleColorsDark();

	float x_scale = 1.0f;
	float y_scale = 1.0f;

	glfwGetWindowContentScale(static_cast<GLFWwindow*>(m_window.GetNativeHandle()), &x_scale, &y_scale);
	float main_scale = x_scale;

	const char* glsl_version = "#version 410 core";
	ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(m_window.GetNativeHandle()), true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void Scene::OnDetach()
{
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Scene::OnUpdate(float time)
{
    ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    DrawFpsPanel(time);
}

void Scene::DrawFpsPanel(float deltaTime)
{
    static double elapsed = 0.0;
    static int frames = 0;
    static float fps = 0.0f;
    static float frameMs = 0.0f;

    elapsed += deltaTime;
    ++frames;

    // Refresh the displayed values about four times per second.
    if (elapsed >= 0.25)
    {
        fps = static_cast<float>(frames / elapsed);
        frameMs = static_cast<float>(elapsed * 1000.0 / frames);

        elapsed = 0.0;
        frames = 0;
    }

    ImGui::Begin("Performance", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Frame time: %.2f ms", frameMs);

    ImGui::End();
}

std::weak_ptr<Entity> Scene::GetEntity(const std::string& name) const
{
    const auto it = m_Entities.find(name);

    if (it == m_Entities.end())
        return {};

    return it->second;
}
}