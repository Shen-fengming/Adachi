#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "Adachi/Renderer/Mesh.h"
#include "Adachi/Renderer/Texture.h"
#include "WeakResourceCache.h"
#include "ResourceLoader.h"
#include "ShaderResource.h"



namespace Adachi {

struct ModelFetchResult
{
    std::shared_ptr<MeshAsset> meshAsset;
};

class Shader;

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
    std::shared_ptr<Texture> FetchTexture(const std::string& texturePath, TextureSemantic semanic, TextureDesc desc);

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

    WeakResourceCache<
        Texture, 
        std::string
    > m_TextureCache;

    ResourceLoader m_ResourceLoader;
};

}