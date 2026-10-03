#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "Adachi/Renderer/Shader.h"
#include "WeakResourceCache.h"

namespace Adachi {

class ResourceManager {
public:
    std::shared_ptr<Shader> GetOrCreateShader(
        const ShaderKey& key)
    {
        return m_ShaderCache.GetOrCreate(
            key,
            [](const ShaderKey& key) {
                return Shader::Create(
                    MakeShaderName(key),
                    key.VertexPath,
                    key.FragmentPath
                );
            }
        );
    }

    std::shared_ptr<Shader> FindShader(
        const ShaderKey& key) const
    {
        return m_ShaderCache.Find(key);
    }

    bool ContainsShader(const ShaderKey& key) const
    {
        return m_ShaderCache.Contains(key);
    }

    std::size_t PurgeExpiredShaders()
    {
        return m_ShaderCache.PurgeExpired();
    }

    void ClearShaderCache()
    {
        m_ShaderCache.Clear();
    }

public:
    static std::string ReadFile(
        const std::filesystem::path& path);

private:
    static std::string MakeShaderName(
        const ShaderKey& key)
    {
        const std::filesystem::path vertexPath(
            key.VertexPath
        );

        const std::filesystem::path fragmentPath(
            key.FragmentPath
        );

        return vertexPath.stem().string() +
               "+" +
               fragmentPath.stem().string();
    }

private:
    WeakResourceCache<
        Shader,
        ShaderKey,
        ShaderKeyHash
    > m_ShaderCache;
};

}