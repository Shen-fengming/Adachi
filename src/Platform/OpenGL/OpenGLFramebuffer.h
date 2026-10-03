#pragma once

#include <cstdint>
#include <glad/gl.h>

#include "Adachi/Renderer/Framebuffer.h"
#include "OpenGLDebug.h"
namespace Adachi
{
class OpenGLFramebuffer: public Framebuffer{
public:
    OpenGLFramebuffer(bool isDefault):m_IsDefault(isDefault)
    {
        if(!m_IsDefault)
        {
            GLCall(glGenFramebuffers(1, &m_RendererID));
        }
        else 
        {
            m_RendererID = 0;
        }
    }
    void Bind() override
    {
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
    }
    void Unbind() override
    {
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

public:
    GLuint ID(){return m_RendererID;}

private:
    GLuint m_RendererID;
    bool m_IsDefault;
};
}