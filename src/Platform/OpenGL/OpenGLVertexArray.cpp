#include <glad/gl.h>
#include <memory>
#include <cstdint>

#include "OpenGLVertexArray.h"
#include "Adachi/Diagnostics/Debug.h"
#include "Adachi/Diagnostics/Log.h"
#include "Adachi/Diagnostics/Profiler.h"
#include "Adachi/Renderer/Buffer.h"
#include "Platform/OpenGL/OpenGLDebug.h"

namespace Adachi{

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderUniformType type)
{
    switch (type)
    {
        case ShaderUniformType::Float:    return GL_FLOAT;
        case ShaderUniformType::Float2:   return GL_FLOAT;
        case ShaderUniformType::Float3:   return GL_FLOAT;
        case ShaderUniformType::Float4:   return GL_FLOAT;
        case ShaderUniformType::Mat3:     return GL_FLOAT;
        case ShaderUniformType::Mat4:     return GL_FLOAT;
        case ShaderUniformType::Int:      return GL_INT;
        case ShaderUniformType::Int2:     return GL_INT;
        case ShaderUniformType::Int3:     return GL_INT;
        case ShaderUniformType::Int4:     return GL_INT;
        case ShaderUniformType::Bool:     return GL_BOOL;
        default:{
            ADACHI_CORE_ASSERT(false, "Unknown ShaderDataType!");
            return 0;
        }
    }
}

OpenGLVertexArray::OpenGLVertexArray(){
    ADACHI_PROFILE_FUNCTION();

    GLCall(glGenVertexArrays(1, &m_RendererID));
}

OpenGLVertexArray::~OpenGLVertexArray(){
    ADACHI_PROFILE_FUNCTION();

    GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void OpenGLVertexArray::Bind() const
{
    ADACHI_PROFILE_FUNCTION();

    glBindVertexArray(m_RendererID);

    //ADACHI_CORE_INFO("VAO {} Binded", m_RendererID);
}

void OpenGLVertexArray::Unbind() const
{
    ADACHI_PROFILE_FUNCTION();

    glBindVertexArray(0);
}

void OpenGLVertexArray::AddVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer){
    ADACHI_PROFILE_FUNCTION();

    ADACHI_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

    Bind();
    vertexBuffer->Bind();

    const auto& layout = vertexBuffer->GetLayout();
    for(const auto& element: layout){
        ADACHI_CORE_INFO("Adding new element of vertex buffer layout:");
        ADACHI_CORE_INFO("  Total Stride: {}", layout.GetStride());
        ADACHI_CORE_INFO("  Size: {}", ShaderDataComponentCount(element.Type));
        ADACHI_CORE_INFO("  Offset: {}", element.Offset);
        ADACHI_CORE_INFO("  VertexBufferIndex: {}", m_VertexBufferIndex);
        switch (element.Type) {
            case ShaderUniformType::Float:
			case ShaderUniformType::Float2:
			case ShaderUniformType::Float3:
			case ShaderUniformType::Float4:
			{
                GLCall(glEnableVertexAttribArray(m_VertexBufferIndex));
                GLCall(glVertexAttribPointer(
                    m_VertexBufferIndex,
                    ShaderDataComponentCount(element.Type),
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    reinterpret_cast<const void*>(static_cast<std::uintptr_t>(element.Offset))
                ));
                m_VertexBufferIndex++;
                break;
            }
            case ShaderUniformType::Int:
            case ShaderUniformType::Int2:
            case ShaderUniformType::Int3:
            case ShaderUniformType::Int4:
            case ShaderUniformType::Bool:
            {
                GLCall(glEnableVertexAttribArray(m_VertexBufferIndex));
                GLCall(glVertexAttribIPointer(
                    m_VertexBufferIndex,
                    ShaderDataTypeSize(element.Type),
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    layout.GetStride(),
                    reinterpret_cast<const void*>(static_cast<std::uintptr_t>(element.Offset))
                ));
                m_VertexBufferIndex++;
                break;
            }
            case ShaderUniformType::Mat3:
            case ShaderUniformType::Mat4:
            {
				uint8_t count = ShaderDataTypeSize(element.Type);
				for (uint8_t i = 0; i < count; i++)
                {
                    glEnableVertexAttribArray(m_VertexBufferIndex);
                    glVertexAttribPointer(m_VertexBufferIndex,
                        count,
                        ShaderDataTypeToOpenGLBaseType(element.Type),
                        element.Normalized ? GL_TRUE : GL_FALSE,
                        layout.GetStride(),
                        (const void*)(element.Offset + sizeof(float) * count * i));
                    glVertexAttribDivisor(m_VertexBufferIndex, 1);
                    m_VertexBufferIndex++;
                }
                break;
            }
            default:
                ADACHI_CORE_ASSERT(false, "Unknown ShaderDataType!");
        }
    }
    m_VertexBuffers.push_back(std::move(vertexBuffer));
}

// void OpenGLVertexArray::SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer){
//     ADACHI_PROFILE_FUNCTION();

//     Bind();
//     indexBuffer->Bind();

//     m_IndexBuffer = std::move(indexBuffer);
// }

void OpenGLVertexArray::PrintOpenGLCurrent()
{
    GLint vao = 0;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao);
    ADACHI_CORE_INFO("VAO: {}", vao);

    GLint maxAttributes = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributes);

    for (GLint location = 0; location < maxAttributes; ++location) {
        GLint enabled = GL_FALSE;
        glGetVertexAttribiv(
            location,
            GL_VERTEX_ATTRIB_ARRAY_ENABLED,
            &enabled
        );

        if (enabled != GL_TRUE)
            continue;

        GLint vertexBuffer = 0;
        GLint size = 0;
        GLint type = 0;
        GLint stride = 0;
        GLint normalized = 0;
        GLint integerAttribute = 0;
        void* offset = nullptr;

        glGetVertexAttribiv(
            location,
            GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING,
            &vertexBuffer
        );
        glGetVertexAttribiv(
            location,
            GL_VERTEX_ATTRIB_ARRAY_SIZE,
            &size
        );
        glGetVertexAttribiv(
            location,
            GL_VERTEX_ATTRIB_ARRAY_TYPE,
            &type
        );
        glGetVertexAttribiv(
            location,
            GL_VERTEX_ATTRIB_ARRAY_STRIDE,
            &stride
        );
        glGetVertexAttribiv(
            location,
            GL_VERTEX_ATTRIB_ARRAY_NORMALIZED,
            &normalized
        );
        glGetVertexAttribiv(
            location,
            GL_VERTEX_ATTRIB_ARRAY_INTEGER,
            &integerAttribute
        );
        glGetVertexAttribPointerv(
            location,
            GL_VERTEX_ATTRIB_ARRAY_POINTER,
            &offset
        );

        ADACHI_CORE_INFO(
        "Attribute {}: VBO={}, components={}, type=0x{:X}, stride={}, offset={}, normalized={}, integer={}",
        location,
        vertexBuffer,
        size,
        type,
        stride,
        offset,
        normalized,
        integerAttribute);
    }
}
}