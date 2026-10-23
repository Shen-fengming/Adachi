#pragma once

#include "Adachi/Renderer/Buffer.h"
#include <string>
#include <filesystem>

class aiScene;
class aiNode;
class aiMesh;

namespace Adachi {

class BufferLayout;

enum class ModelPart : std::uint32_t {
    None       = 0,
    Meshes     = 1 << 0,
    Materials  = 1 << 1,
    Textures   = 1 << 2,
    Skeleton   = 1 << 3,
    Animations = 1 << 4
};

constexpr ModelPart operator|(ModelPart lhs, ModelPart rhs)
{
    return static_cast<ModelPart>(
        static_cast<std::uint32_t>(lhs) |
        static_cast<std::uint32_t>(rhs)
    );
}

constexpr bool HasPart(ModelPart value, ModelPart part)
{
    return (static_cast<std::uint32_t>(value) &
            static_cast<std::uint32_t>(part)) != 0;
}

constexpr ModelPart RemoveParts(
    ModelPart value,
    ModelPart partsToRemove) noexcept
{
    using Value = std::underlying_type_t<ModelPart>;

    return static_cast<ModelPart>(
        static_cast<Value>(value) &
        static_cast<Value>(~static_cast<Value>(partsToRemove))
    );
}

struct ModelOutputForMesh{
    BufferLayout Bufferlayout;
    std::vector<float> VertexData;
    std::vector<std::uint16_t> Indices;

    ModelOutputForMesh(BufferLayout bufferlayout, std::vector<float> vertexdata,std::vector<std::uint16_t> indices):
    Bufferlayout(std::move(bufferlayout)),VertexData(std::move(vertexdata)), Indices(std::move(indices))
    {}
};

struct ModelLoadOutput{
    ModelPart RequestedParts = ModelPart::None;
    ModelPart LoadedParts = ModelPart::None;

    std::vector<ModelOutputForMesh> ForMeshes;
};

class ResourceLoader
{
public:
    std::string LoadString(const std::filesystem::path& path);
    ModelLoadOutput LoadModel(const std::filesystem::path& path, const ModelPart& modelPart);

private:
    void DumpNode(const aiScene& scene, const aiNode& node, std::ostream& output, std::size_t depth);
    void DumpScene(const aiScene& scene, std::ostream& output);
    ModelOutputForMesh ProcessMesh(const aiMesh& mesh);
};

}