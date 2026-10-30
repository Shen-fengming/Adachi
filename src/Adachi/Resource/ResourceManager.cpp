#include "ResourceManager.h"
#include "Adachi/Diagnostics/Debug.h"
#include "Adachi/Diagnostics/Log.h"
#include "Adachi/Renderer/Buffer.h"
#include "Adachi/Renderer/Mesh.h"
#include "Adachi/Renderer/Shader.h"
#include "Adachi/Renderer/Texture.h"
#include "Adachi/Renderer/VertexArray.h"
#include "Adachi/Resource/ResourceLoader.h"
#include <memory>
#include <utility>

namespace Adachi {

std::shared_ptr<Shader> ResourceManager::FetchShader(const std::string& vsPath, const std::string& fsPath)
{
    ShaderKey key = ShaderKey::FromPaths(vsPath, fsPath);

    return m_ShaderCache.GetOrCreate(
        key, 
        [this](const ShaderKey& key)
        {
            auto shaderDirectory = m_ResourcePath / "shaders";

            auto resolvedVertexPath = shaderDirectory / key.VertexPath;
            auto resolvedFragmentPath = shaderDirectory / key.FragmentPath;

            auto vertexSource = m_ResourceLoader.LoadString(resolvedVertexPath);
            auto fragmentSource = m_ResourceLoader.LoadString(resolvedFragmentPath);

            return Shader::Create(vertexSource.c_str(), fragmentSource.c_str());
        }
    );
}

ModelFetchResult ResourceManager::FetchFromModel(const std::string& modelPath, const ModelPart& modelPart)
{
    ModelPart toLoad = modelPart;
    // according to flag make key path & contains
    if(HasPart(modelPart, ModelPart::Meshes) && m_MeshAssetCache.Find(modelPath))
    {
        toLoad = RemoveParts(toLoad, ModelPart::Meshes);
    }

    // call loader, get the resources 
    auto resolvedModelPath = m_ResourcePath / "models" / modelPath;
    auto loadResult = m_ResourceLoader.LoadModel(resolvedModelPath, modelPart);
    
    ModelFetchResult result;
    // take such resources to make gpu-side resources using GetOrFind
    if(HasPart(modelPart, ModelPart::Meshes))
    {
        result.meshAsset = m_MeshAssetCache.GetOrCreate(
            modelPath,
            [&loadResult](const std::string& key)
            {
                auto result = std::make_shared<MeshAsset>();
                result->Meshes.reserve(loadResult.ForMeshes.size());

                for(auto& loadedMesh: loadResult.ForMeshes)
                {
                    auto tempVB = VertexBuffer::Create(loadedMesh.VertexData.data(), loadedMesh.VertexData.size()*sizeof(loadedMesh.VertexData[0]), std::move(loadedMesh.Bufferlayout));
                    auto tempVA = VertexArray::Create();
                    tempVA->AddVertexBuffer(std::move(tempVB));

                    auto tempIB = IndexBuffer::Create(loadedMesh.Indices.data(), loadedMesh.Indices.size(), IndexType::UInt16);

                    result->Meshes.emplace_back( Mesh::Create("loaded", std::move(tempVA),std::move(tempIB)));
                }

                return result;
            }
        );
    }

    return result;
}

std::shared_ptr<Texture> ResourceManager::FetchTexture(const std::string& texturePath, TextureSemantic semanic, TextureDesc desc)
{
    return m_TextureCache.GetOrCreate(
        texturePath, 
        [&](const std::string& key)
        {
            auto resolvedTexturePath = m_ResourcePath / "textures" / key;
            auto loadResult = m_ResourceLoader.LoadTexture(resolvedTexturePath);

            return Texture::Create(loadResult, semanic, desc);
        }
    );
}

} // namespace Adachi