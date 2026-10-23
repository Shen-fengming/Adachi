#pragma once

#include "Adachi/Core/Window.h"
#include "Adachi/Scene/Scene.h"

#include "Adachi/Event/Event.h"

class ModelLoaderScene: public Adachi::Scene{
public:
    ModelLoaderScene(Adachi::ResourceManager& resourceManager, Adachi::Window& window);

    void OnAttach() override;
    void OnDetach() override;
    void OnEvent(Adachi::Event& event) override {}
    void OnUpdate(float time) override;

private:
    glm::vec3 m_BunnyPosition{0.0f,0.0f,-3.0f};
    glm::vec3 m_BunnyRotation{0.0f};
    glm::vec3 m_BunnyScale{1.0f};

    glm::vec3 m_BunnyColor{1.0f};
};