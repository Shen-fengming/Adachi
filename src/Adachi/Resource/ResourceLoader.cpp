#include "ResourceLoader.h"
#include "Adachi/Diagnostics/Log.h"
#include "Adachi/Diagnostics/Debug.h"
#include "Adachi/Renderer/Buffer.h"

#include <cstddef>
#include <fstream>
#include <sstream>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/config.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>


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

ModelOutputForMesh ResourceLoader::ProcessMesh(const aiMesh& mesh)
{
    const bool hasPositions = mesh.HasPositions();
    const bool hasNormals = mesh.HasNormals();

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
    BufferLayout bufferLayout{std::move(elements)};

    std::vector<float> vertexData;
    vertexData.reserve(static_cast<std::size_t>(mesh.mNumVertices) * dataNum);
    for (unsigned int i = 0; i < mesh.mNumVertices; i++)
    {
        if(hasPositions)
        {
            const aiVector3D& position = mesh.mVertices[i];

            vertexData.push_back(position.x);
            vertexData.push_back(position.y);
            vertexData.push_back(position.z);
        }
        if(hasNormals)
        {
            const aiVector3D& normal = mesh.mNormals[i];

            vertexData.push_back(normal.x);
            vertexData.push_back(normal.y);
            vertexData.push_back(normal.z);
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

    DumpScene(*imported, std::cout);
    
    if (HasPart(modelPart, ModelPart::Meshes))
    {
        result.ForMeshes.reserve(imported->mNumMeshes);
        for (unsigned int index = 0; index < imported->mNumMeshes; ++index) {
            result.ForMeshes.emplace_back(ProcessMesh(*imported->mMeshes[index]));
        }

        result.LoadedParts =  result.LoadedParts | ModelPart::Meshes;
    }

    

    return result;
}
}