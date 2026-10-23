#include "Entity.h"

namespace Adachi 
{

void Entity::CalculateTransform()
{
    glm::mat4 transform{1.0f};

    transform = glm::translate(transform, m_Position);
    transform *= glm::rotate(glm::mat4{1.0f}, m_Rotation.z, glm::vec3{0, 0, 1});
    transform *= glm::rotate(glm::mat4{1.0f}, m_Rotation.y, glm::vec3{0, 1, 0});
    transform *= glm::rotate(glm::mat4{1.0f}, m_Rotation.x, glm::vec3{1, 0, 0});
    transform = glm::scale(transform, m_Scale);

    m_Transform = transform;
}

void Entity::SetPosition(const glm::vec3& position)
{
    m_Position = position;
    CalculateTransform();
}

void Entity::SetRotation(const glm::vec3& rotation)
{
    m_Rotation = rotation;
    CalculateTransform();
}

void Entity::SetScale(const glm::vec3& scale)
{
    m_Scale = scale;
    CalculateTransform();
}

}