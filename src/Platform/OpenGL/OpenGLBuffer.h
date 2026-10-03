// src/Platform/OpenGL/OpenGLIndexBuffer.h
#pragma once

#include <cstdint>
#include <glad/gl.h>

#include "Adachi/Renderer/Buffer.h"


namespace Adachi {
class OpenGLVertexBuffer: public VertexBuffer{
public:
    explicit OpenGLVertexBuffer(const void* data, uint32_t size);
    ~OpenGLVertexBuffer();

    void Bind() const override;
    void Unbind()const override;

    const BufferLayout& GetLayout() const override {return m_BufferLayout;}
    void SetLayout(BufferLayout& bufferlayout) override {m_BufferLayout = bufferlayout;}
private:
    uint32_t m_RendererID;
    BufferLayout m_BufferLayout;
};

class OpenGLIndexBuffer: public IndexBuffer
{
public:
    OpenGLIndexBuffer(std::span<const std::uint16_t> indices, uint32_t count);

    ~OpenGLIndexBuffer() override;

    void Bind() const override;
    void UnBind() const override;
    std::uint32_t GetCount() const override {return m_Count;};
    GLuint GetID(){return m_RendererID;}

    static void PrintOpenGLCurrent();

private:
    unsigned int m_RendererID = 0;
    std::uint32_t m_Count = 0;
};

}