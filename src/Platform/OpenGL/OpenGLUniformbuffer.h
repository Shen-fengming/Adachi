#pragma once

#include <glad/gl.h>

#include "Adachi/Diagnostics/Debug.h"
#include "Adachi/Renderer/UniformBuffer.h"
#include "Platform/OpenGL/OpenGLDebug.h"

namespace Adachi {

class OpenGLUniformbuffer: public UniformBuffer{
public:
    OpenGLUniformbuffer(UniformBufferLayout& layout, uint32_t bindingpoint):UniformBuffer(layout,bindingpoint)
    {
        GLCall(glGenBuffers(1, &m_RendererID));
        GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID));
        GLCall(glBufferData(GL_UNIFORM_BUFFER, layout.CurrentOffset, NULL, GL_STATIC_DRAW));
        GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, bindingpoint, m_RendererID));
        GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    }
    ~OpenGLUniformbuffer()
    {
        GLCall(glDeleteBuffers(1, &m_RendererID));
    }

    void SetLayout(UniformBufferLayout& layout) override
    {
        m_Layout = layout;
    }

    const GLuint GetID() const {return m_RendererID;}

    

    // void SetData(const void* data, uint32_t size, uint32_t offset = 0) override
    // {
    //     GLCall(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data) );
    // }

    

    // bool operator==(const UniformBuffer& other) const override
    // {
    //     const auto* uniformBuffer = static_cast<const OpenGLUniformbuffer*>(&other);
    //     return uniformBuffer->GetID() == m_RendererID;
    // }

private:
    GLuint m_RendererID;
    
};

}