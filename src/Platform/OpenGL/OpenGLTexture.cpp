#include "Adachi/Diagnostics/Debug.h"
#include "OpenGLTexture.h"
#include "OpenGLDebug.h"

namespace Adachi {

static GLint ToGL(TextureWrap wrap)
{
    switch (wrap) {
        case TextureWrap::Repeat:         return GL_REPEAT;
        case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
        case TextureWrap::ClampToEdge:    return GL_CLAMP_TO_EDGE;
        case TextureWrap::ClampToBorder:  return GL_CLAMP_TO_BORDER;
    }
    ADACHI_CORE_ASSERT(false, "Unknown texture wrap");
}

static GLint ToGL(TextureMinFilter filter)
{
    switch (filter) {
        case TextureMinFilter::Nearest:              return GL_NEAREST;
        case TextureMinFilter::Linear:               return GL_LINEAR;
        case TextureMinFilter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
        case TextureMinFilter::LinearMipmapNearest:  return GL_LINEAR_MIPMAP_NEAREST;
        case TextureMinFilter::NearestMipmapLinear:  return GL_NEAREST_MIPMAP_LINEAR;
        case TextureMinFilter::LinearMipmapLinear:   return GL_LINEAR_MIPMAP_LINEAR;
    }
    ADACHI_CORE_ASSERT(false, "Unknown minification filter");
}

static GLint ToGL(TextureMagFilter filter)
{
    switch (filter) {
        case TextureMagFilter::Nearest: return GL_NEAREST;
        case TextureMagFilter::Linear:  return GL_LINEAR;
    }
    ADACHI_CORE_ASSERT(false, "Unknown magnification filter");
}

static GLint ToGL(TextureFormat format)
{
    switch (format) {
        case TextureFormat::R8:     return GL_R8;
        case TextureFormat::RGB8:   return GL_RGB8;
        case TextureFormat::RGBA8:  return GL_RGBA8;
        case TextureFormat::SRGB8:  return GL_SRGB8;
        case TextureFormat::SRGBA8: return GL_SRGB8_ALPHA8;
    }
    ADACHI_CORE_ASSERT(false, "Unknown texture format");
}

OpenGLTexture::OpenGLTexture(const ImageData& image, TextureSemantic semantic, TextureDesc desc):
m_TextureUnit(static_cast<uint32_t>(semantic))
{
    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        ToGL(desc.format),       // GPU storage format
        image.width,
        image.height,
        0,
        GL_RGBA,                 // CPU data layout: always RGBA
        GL_UNSIGNED_BYTE,
        image.pixels.data()
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ToGL(desc.wrapS));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ToGL(desc.wrapT));

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ToGL(desc.minFilter));
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ToGL(desc.magFilter));

    if (desc.options == TextureOption::GenerateMipmaps) 
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    } 
    else 
    {
        // Only level 0 exists; constrain sampling to that level.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    }
}

OpenGLTexture::~OpenGLTexture()
{
    GLCall(glDeleteTextures(1, &m_RendererID));
}

void OpenGLTexture::Bind()
{
    GLCall(glActiveTexture(GL_TEXTURE0 + m_TextureUnit));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void OpenGLTexture::UnBind()
{
    GLCall(glActiveTexture(GL_TEXTURE0 + m_TextureUnit));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
}