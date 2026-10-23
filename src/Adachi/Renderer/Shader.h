#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <cstddef>

namespace Adachi{

class Shader
{
public:
    virtual ~Shader() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    static std::shared_ptr<Shader> Create(const char* vertexShaderCode, const char* fragmentShaderCode);
};

}