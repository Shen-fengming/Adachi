#pragma once

#include "Adachi/Renderer/UniformValue.h"

#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <cstddef>
#include <filesystem>
#include <functional>

namespace Adachi{

struct ShaderKey {
    std::string VertexPath;
    std::string FragmentPath;

    static ShaderKey FromPaths(
        const std::filesystem::path& vertexPath,
        const std::filesystem::path& fragmentPath)
    {
        return {
            vertexPath.lexically_normal().generic_string(),
            fragmentPath.lexically_normal().generic_string()
        };
    }

    bool operator==(const ShaderKey& other) const noexcept
    {
        return VertexPath == other.VertexPath &&
               FragmentPath == other.FragmentPath;
    }
};

struct ShaderKeyHash {
    std::size_t operator()(const ShaderKey& key) const noexcept
    {
        std::size_t result =
            std::hash<std::string>{}(key.VertexPath);

        const std::size_t fragmentHash =
            std::hash<std::string>{}(key.FragmentPath);

        result ^= fragmentHash +
                  static_cast<std::size_t>(0x9e3779b9U) +
                  (result << 6U) +
                  (result >> 2U);

        return result;
    }
};

class Shader
{
public:
    virtual ~Shader() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual const std::string& GetName() const = 0;

    static std::shared_ptr<Shader> Create(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);

    // Cache of currently set uniform value's place and value

};

}