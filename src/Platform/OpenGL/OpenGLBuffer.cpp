#include <cstdint>
#include <glad/gl.h>

#include "Adachi/Diagnostics/Log.h"
#include "Adachi/Diagnostics/Profiler.h"
#include "OpenGLBuffer.h"
#include "Adachi/Diagnostics/Profiler.h"
#include "OpenGLDebug.h"

namespace Adachi {
#pragma region VertexBuffer
OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, uint32_t size, BufferLayout bufferlayout){
    ADACHI_PROFILE_FUNCTION();

    GLCall(glGenBuffers(1, &m_RendererID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));

    m_BufferLayout = std::move(bufferlayout);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer(){
    ADACHI_PROFILE_FUNCTION();

    GLCall(glDeleteBuffers(1, &m_RendererID));
    ADACHI_CORE_INFO("Vertex Buffer destructed");
}

void OpenGLVertexBuffer::OpenGLVertexBuffer::Bind() const{
    ADACHI_PROFILE_FUNCTION();
    
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    ADACHI_CORE_INFO("VBO {} Binded", m_RendererID);
}

void OpenGLVertexBuffer::OpenGLVertexBuffer::Unbind() const{
    ADACHI_PROFILE_FUNCTION();

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

#pragma endregion

#pragma region IndexBuffer
OpenGLIndexBuffer::OpenGLIndexBuffer(const void* data, uint32_t count, IndexType indextype):m_Count(count), m_IndexType(indextype){
    GLCall(glGenBuffers(1, &m_RendererID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));

    const std::size_t sizeInBytes = static_cast<std::size_t>(count) * GetIndexTypeSize(m_IndexType);
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeInBytes), data, GL_STATIC_DRAW));
}

OpenGLIndexBuffer::~OpenGLIndexBuffer(){
    GLCall(glDeleteBuffers(1, &m_RendererID));
    ADACHI_CORE_INFO("Index Buffer destructed");
    //std::cout << "Vertex Buffer destructed" << std::endl;
}

void OpenGLIndexBuffer::Bind() const{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));

    ADACHI_CORE_INFO("IBO {} Binded", m_RendererID);
}

void OpenGLIndexBuffer::UnBind() const{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void OpenGLIndexBuffer::PrintOpenGLCurrent()
{
    GLint indexBuffer = 0;
    GLint64 indexBufferSize = 0;
    
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &indexBuffer);
    if(indexBuffer>0)
    {
        glGetBufferParameteri64v(
            GL_ELEMENT_ARRAY_BUFFER,
            GL_BUFFER_SIZE,
            &indexBufferSize
        );
    }

    ADACHI_CORE_INFO("IBO: {}, bytes = {}", indexBuffer, indexBufferSize);
}
#pragma endregion
} // namespace Adachi