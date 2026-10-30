#include "Adachi/Renderer/Texture.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <glad/gl.h>

namespace Adachi {

class OpenGLTexture : public Texture
{
public:
    OpenGLTexture(const ImageData& data, TextureSemantic semantic, TextureDesc desc);
    ~OpenGLTexture() override;

    GLuint GetID(){ return m_RendererID;}
    GLuint GetTextureUnit(){return m_TextureUnit;}
    void Bind() override;
    void UnBind() override;

    static void ActiveTexture(uint32_t textureunit);
private:
    uint32_t m_RendererID;
    uint32_t m_TextureUnit;
};

}