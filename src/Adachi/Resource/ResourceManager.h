#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "Adachi/Renderer/Shader.h"
#include "Adachi/Renderer/Mesh.h"
#include "WeakResourceCache.h"
#include "ResourceLoader.h"
#include "ShaderResource.h"


namespace Adachi {

struct ModelFetchResult
{
    std::shared_ptr<MeshAsset> meshAsset;
};

class ResourceManager {
public:
    struct Config
    {
        std::filesystem::path ResourcePath;
    };

    ResourceManager(Config config): m_ResourcePath(std::move(config.ResourcePath)){}

    std::shared_ptr<Shader> FetchShader(const std::string& vsPath, const std::string& fsPath);

    // The result of loading a model should be a set of outcome: meshes, materials, etc
    // Should material be immutable?
    // should have a parameter input about what to load from this model
    ModelFetchResult FetchFromModel(const std::string& modelPath, const ModelPart& modelPart);

private:
    std::filesystem::path m_ResourcePath;

    WeakResourceCache<
        Shader,
        ShaderKey,
        ShaderKeyHash
    > m_ShaderCache;

    WeakResourceCache<
        MeshAsset,
        std::string
    > m_MeshAssetCache;

    

    ResourceLoader m_ResourceLoader;
};

}