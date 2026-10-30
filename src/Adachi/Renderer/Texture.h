#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace Adachi 
{
enum class TextureSemantic {
    BaseColor,
    Normal,
    MetallicRoughness,
    Emission
};

#pragma region Texture Description
enum class TextureOption : std::uint32_t {
    None            = 0,
    GenerateMipmaps = 1u << 0,
};

enum class TextureFormat {
    R8,
    RGB8,
    RGBA8,
    SRGB8,
    SRGBA8,
};

enum class TextureWrap {
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
};

enum class TextureMinFilter {
    Nearest,
    Linear,
    NearestMipmapNearest,
    LinearMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapLinear,
};

enum class TextureMagFilter {
    Nearest,
    Linear,
};

struct TextureDesc {
    TextureFormat format = TextureFormat::RGBA8;

    TextureWrap wrapS = TextureWrap::Repeat;
    TextureWrap wrapT = TextureWrap::Repeat;

    TextureMinFilter minFilter = TextureMinFilter::LinearMipmapLinear;
    TextureMagFilter magFilter = TextureMagFilter::Linear;

    TextureOption options = TextureOption::GenerateMipmaps;
};
#pragma endregion

struct ImageData {
    uint32_t width = 0;
    uint32_t height = 0;

    // RGBA8, tightly packed, rows ordered top to bottom.
    // Size must equal width * height * 4.
    std::vector<uint8_t> pixels;

    size_t rowBytes() const {
        return static_cast<size_t>(width) * 4;
    }
};

class Texture
{
public:
    virtual ~Texture() = default;

    virtual void Bind() = 0;
    virtual void UnBind() = 0;

    static std::shared_ptr<Texture> Create(ImageData& data, TextureSemantic semanic, TextureDesc desc);
};
}