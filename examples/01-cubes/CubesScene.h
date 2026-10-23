// --Static stuff--
// Shader source
// Point source

#pragma once

#include "Adachi/Resource/ResourceManager.h"
#include "Adachi/Scene/Scene.h"

#include "Adachi/Event/Event.h"
#include "Adachi/Renderer/RenderEnums.h"

class CubeScene: public Adachi::Scene{
public:
    CubeScene(Adachi::ResourceManager& resourceManager, Adachi::Window& window);

    void OnAttach() override;
    void OnDetach() override;
    void OnEvent(Adachi::Event& event) override {}
    void OnUpdate(float time) override;

private:
// temp that should be managed by a ResourceManager in future
// Now they are just arguments to initialize Meshes&Materials
// After successfully setting Mesh&Materials, the shared_ptr should reset
    std::vector<std::shared_ptr<Adachi::Material>> m_Materials;
private:
    Adachi::PrimitiveTopology m_currentTopology = Adachi::PrimitiveTopology::TriangleList;
    bool m_topologyChanged = false;
    int selectedIndex = 0;

    bool m_R = true;
    bool m_G = true;
    bool m_B = true;
    bool m_A = true;
    bool m_colorWriteChanged = false;
};