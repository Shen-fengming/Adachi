#pragma once

#include "Adachi/Core/Window.h"
#include "Adachi/Scene/Scene.h"

#include "Adachi/Event/Event.h"

class GrassScene: public Adachi::Scene{
public:
    GrassScene(Adachi::ResourceManager& resourceManager, Adachi::Window& window);

    void OnAttach() override;
    void OnDetach() override;
    void OnEvent(Adachi::Event& event) override {}
    void OnUpdate(float time) override;

private:
    glm::vec3 m_GrassColor{1.0f};

    glm::vec3 m_GrassPosition{0.0f,0.0f,-10.0f};
    glm::vec3 m_GrassRotation{0.0f};
    glm::vec3 m_GrassScale{1.0f};
};