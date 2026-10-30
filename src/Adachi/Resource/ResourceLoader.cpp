#include "ResourceLoader.h"
#include "Adachi/Diagnostics/Log.h"
#include "Adachi/Diagnostics/Debug.h"
#include "Adachi/Renderer/Buffer.h"
#include "Adachi/Renderer/Texture.h"

#include <cstddef>
#include <fstream>
#include <memory>
#include <sstream>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/config.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>


namespace Adachi{

std::string ResourceLoader::LoadString(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);
    ADACHI_CORE_ASSERT(
        file.is_open(),
        "Failed to open resource: {}",
        path.string()
    );

    std::ostringstream contents;
    contents << file.rdbuf();
    ADACHI_CORE_ASSERT(
        !contents.bad(),
        "Failed to read resource: {}",
        path.string()
    );

    return std::move(contents).str();
}

void ResourceLoader::DumpNode(
        const aiScene& scene,
        const aiNode& node,
        std::ostream& output,
        std::size_t depth)
{
    const std::string indent(depth * 2, ' ');

    output
        << indent
        << "Node: " << node.mName.C_Str()
        << " | meshes: " << node.mNumMeshes
        << " | children: " << node.mNumChildren
        << '\n';

    for (unsigned int i = 0; i < node.mNumMeshes; ++i)
    {
        const unsigned int meshIndex = node.mMeshes[i];

        if (meshIndex >= scene.mNumMeshes)
        {
            output
                << indent
                << "  Invalid mesh index: "
                << meshIndex
                << '\n';

            continue;
        }

        const aiMesh& mesh = *scene.mMeshes[meshIndex];

        output
            << indent
            << "  Mesh[" << meshIndex << "]: "
            << mesh.mName.C_Str()
            << '\n';

        output
            << indent
            << "    vertices: " << mesh.mNumVertices
            << ", faces: " << mesh.mNumFaces
            << ", bones: " << mesh.mNumBones
            << ", material: " << mesh.mMaterialIndex
            << '\n';

        output
            << indent
            << "    normals: " << mesh.HasNormals()
            << ", tangents: " << mesh.HasTangentsAndBitangents()
            << ", uv0: " << mesh.HasTextureCoords(0)
            << ", colors0: " << mesh.HasVertexColors(0)
            << '\n';
    }

    for (unsigned int i = 0; i < node.mNumChildren; ++i)
    {
        DumpNode(
            scene,
            *node.mChildren[i],
            output,
            depth + 1
        );
    }
}

void ResourceLoader::DumpScene(const aiScene& scene, std::ostream& output)
{
    output << "Scene summary\n";
    output << "  meshes:     " << scene.mNumMeshes << '\n';
    output << "  materials:  " << scene.mNumMaterials << '\n';
    output << "  animations: " << scene.mNumAnimations << '\n';
    output << "  textures:   " << scene.mNumTextures << '\n';
    output << "  lights:     " << scene.mNumLights << '\n';
    output << "  cameras:    " << scene.mNumCameras << '\n';

    output << "\nNode hierarchy\n";

    if (scene.mRootNode)
        DumpNode(scene, *scene.mRootNode, output, 0);
}

ModelOutputForMesh ResourceLoader::ProcessMesh(const aiMesh& mesh, const aiMatrix4x4& transform)
{
    const bool hasPositions = mesh.HasPositions();
    const bool hasNormals = mesh.HasNormals();
    const bool hasTextureCoordinates = mesh.HasTextureCoords(0);

    std::vector<BufferElement> elements;
    size_t dataNum{0};
    if(hasPositions)
    {
        elements.emplace_back(
            "a_Position",
            ShaderUniformType::Float3
        );
        dataNum+=3;
    }
    if(hasNormals)
    {
        elements.emplace_back(
            "a_Normal",
            ShaderUniformType::Float3
        );
        dataNum+=3;
    }
    if(hasTextureCoordinates)
    {
        elements.emplace_back(
            "a_TexCoord",
            ShaderUniformType::Float2
        );
    }
    BufferLayout bufferLayout{std::move(elements)};

    std::vector<float> vertexData;
    vertexData.reserve(static_cast<std::size_t>(mesh.mNumVertices) * dataNum);

    aiMatrix3x3 normalTransform = aiMatrix3x3(transform);
    normalTransform.Inverse().Transpose();

    for (unsigned int i = 0; i < mesh.mNumVertices; i++)
    {
        if(hasPositions)
        {
            //const aiVector3D& position = mesh.mVertices[i];
            const aiVector3D position = transform * mesh.mVertices[i];

            vertexData.push_back(position.x);
            vertexData.push_back(position.y);
            vertexData.push_back(position.z);
        }
        if(hasNormals)
        {
            //const aiVector3D& normal = mesh.mNormals[i];
            aiVector3D normal = normalTransform * mesh.mNormals[i];
            normal.Normalize();

            vertexData.push_back(normal.x);
            vertexData.push_back(normal.y);
            vertexData.push_back(normal.z);
        }
        if(hasNormals)
        {
            const aiVector3D& uv = mesh.mTextureCoords[0][i];

            vertexData.push_back(uv.x); // U
            vertexData.push_back(uv.y); // V
        }
    }

    std::vector<std::uint16_t> indices;
    indices.reserve(mesh.mNumFaces * 3);
    for (unsigned int i = 0; i < mesh.mNumFaces; ++i)
    {
        const aiFace& face = mesh.mFaces[i];

        ADACHI_CORE_ASSERT(
            face.mNumIndices == 3,
            "Expected triangle face, but face {} contains {} indices.",
            i,
            face.mNumIndices
        );

        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }

    ModelOutputForMesh result(std::move(bufferLayout), std::move(vertexData), std::move(indices));
    return result;
}

ModelLoadOutput ResourceLoader::LoadModel(const std::filesystem::path& path, const ModelPart& modelPart)
{
    ADACHI_CORE_ASSERT(!path.empty(), "Cannot load a model from an empty path.");

    ModelLoadOutput result;

    // If choose to load it using assimp
    Assimp::Importer importer;
    importer.SetPropertyInteger(
        AI_CONFIG_PP_SLM_VERTEX_LIMIT,
        std::numeric_limits<std::uint16_t>::max());

    unsigned int flags =
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ImproveCacheLocality |
        aiProcess_SortByPType |
        aiProcess_SplitLargeMeshes |
        aiProcess_ValidateDataStructure;

    const aiScene* imported = importer.ReadFile(path.string(),flags);
    if (!imported ||
    (imported->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0 ||
    !imported->mRootNode)
    {
        ADACHI_CORE_ERROR(
            "Failed to import model {}: {}",
            path.string(),
            importer.GetErrorString()
        );

        return ModelLoadOutput{};
    }

    
    float importScale = 0.01f;
    if (imported->mMetaData)
    {
        float unitScaleFactor = 1.0f;
        if (imported->mMetaData->Get("UnitScaleFactor", unitScaleFactor))
        {
            ADACHI_CORE_INFO(
                "FBX units: {} centimeters per file unit",
                unitScaleFactor
            );
        }
        else
        {
            ADACHI_CORE_WARN("No FBX unit metadata found, assume 1 centimeters per file unit");
        }
        importScale *= unitScaleFactor;
    }
    aiMatrix4x4 rootTransform;
    aiMatrix4x4::Scaling(
        aiVector3D(importScale, importScale, importScale),
        rootTransform
    );

    DumpScene(*imported, std::cout);
    
    if (HasPart(modelPart, ModelPart::Meshes))
    {
        result.ForMeshes.reserve(imported->mNumMeshes);

        auto processNode = [&](auto&& self,
                       const aiNode& node,
                       const aiMatrix4x4& parentTransform) -> void
        {
            const aiMatrix4x4 worldTransform = parentTransform * node.mTransformation;

            // Process the meshes referenced by this node.
            for (unsigned int i = 0; i < node.mNumMeshes; ++i)
            {
                const unsigned int meshIndex = node.mMeshes[i];

                result.ForMeshes.emplace_back(
                    ProcessMesh(*imported->mMeshes[meshIndex], worldTransform)
                );
            }

            // Children inherit this node's accumulated transform.
            for (unsigned int i = 0; i < node.mNumChildren; ++i)
            {
                self(self, *node.mChildren[i], worldTransform);
            }
        };

        // Assimp's default matrix constructor produces identity.
        processNode(processNode, *imported->mRootNode, rootTransform);

        result.LoadedParts =  result.LoadedParts | ModelPart::Meshes;
    }

    return result;
}

ImageData ResourceLoader::LoadTexture(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    ADACHI_CORE_ASSERT(file, "Cannot open texture: {}", path.string());

    const auto end = file.tellg();
    ADACHI_CORE_ASSERT(end > 0, "Texture file is empty: {}", path.string());
    ADACHI_CORE_ASSERT(
        end <= static_cast<std::streamoff>(std::numeric_limits<int>::max()), 
        "Texture file is too large: {}", 
        path.string());
    const auto validFileSize = static_cast<size_t>(end);

    std::vector<unsigned char> encoded(validFileSize);
    file.seekg(0, std::ios::beg);
    auto success = bool{file.read(reinterpret_cast<char*>(encoded.data()), static_cast<std::streamsize>(encoded.size()))};
    ADACHI_CORE_ASSERT(success, "Cannot read texture: ",  path.string());
    
    int width = 0;
    int height = 0;
    int sourceChannels = 0;
    
    std::unique_ptr<unsigned char, decltype(&stbi_image_free)> decoded(
        stbi_load_from_memory(
            encoded.data(),
            static_cast<int>(encoded.size()),
            &width,
            &height,
            &sourceChannels,
            STBI_rgb_alpha),
        &stbi_image_free);
    if(!decoded)
    {
        const char* reason = stbi_failure_reason();
        ADACHI_CORE_ASSERT(decoded, "Cannot decode texture: {} ({})", path.string(), (reason ? reason : "unknown error"));
    }

    ADACHI_CORE_ASSERT(width > 0 && height > 0, "Invalid texture dimensions");
    const size_t w = static_cast<size_t>(width);
    const size_t h = static_cast<size_t>(height);
    ADACHI_CORE_ASSERT(w <= std::numeric_limits<size_t>::max() / 4 / h, "Decoded texture is too large" );
    const size_t byteCount = w * h * 4;

    ImageData image;
    image.width = static_cast<uint32_t>(width);
    image.height = static_cast<uint32_t>(height);
    image.pixels.assign(decoded.get(), decoded.get() + byteCount);

    return image;
}
}