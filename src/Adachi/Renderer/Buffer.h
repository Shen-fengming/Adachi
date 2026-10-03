// src/Adachi/Renderer/IndexBuffer.cpp
// This header file is based on Hazel
#pragma once

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <span>
#include <string>

#include "Adachi/Diagnostics/Debug.h"

namespace Adachi {
enum class ShaderUniformType
{
    None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

// Measure stride
static uint32_t ShaderDataTypeSize(ShaderUniformType type)
{
    switch (type)
    {
        case ShaderUniformType::Float:    return 4;
        case ShaderUniformType::Float2:   return 4 * 2;
        case ShaderUniformType::Float3:   return 4 * 3;
        case ShaderUniformType::Float4:   return 4 * 4;
        case ShaderUniformType::Mat3:     return 4 * 3 * 3;
        case ShaderUniformType::Mat4:     return 4 * 4 * 4;
        case ShaderUniformType::Int:      return 4;
        case ShaderUniformType::Int2:     return 4 * 2;
        case ShaderUniformType::Int3:     return 4 * 3;
        case ShaderUniformType::Int4:     return 4 * 4;
        case ShaderUniformType::Bool:     return 1;
        default: {
            ADACHI_CORE_ASSERT(false, "Unknown ShaderDataType!");
            return 0;
        }
    }
}

// Measure size(component count)
static uint32_t ShaderDataComponentCount(ShaderUniformType type)
{
    switch (type)
    {
        case ShaderUniformType::Float:    return 1;
        case ShaderUniformType::Float2:   return 2;
        case ShaderUniformType::Float3:   return 3;
        case ShaderUniformType::Float4:   return 4;
        case ShaderUniformType::Mat3:     return 3; //3*float3
        case ShaderUniformType::Mat4:     return 4; //4*float4
        case ShaderUniformType::Int:      return 1;
        case ShaderUniformType::Int2:     return 2;
        case ShaderUniformType::Int3:     return 3;
        case ShaderUniformType::Int4:     return 4;
        case ShaderUniformType::Bool:     return 1;
        default: {
            ADACHI_CORE_ASSERT(false, "Unknown ShaderDataType!");
            return 0;
        }
    }
}


struct BufferElement{
    std::string Name;
    ShaderUniformType Type;
    uint32_t Stride;      //Stride
    bool Normalized;
    uint32_t Offset;

    BufferElement(const std::string& name, ShaderUniformType type, bool normalized = false):
        Name(name),
        Type(type),
        Stride(ShaderDataTypeSize(type)),
        Normalized(normalized)
    {}

};

class BufferLayout{
public:
    BufferLayout(){}
    BufferLayout(std::initializer_list<BufferElement> elements):m_Elements(elements){
        uint32_t offset = 0;
        m_Stride = 0;

        for(auto& element: m_Elements){
            element.Offset = offset;
            offset+=element.Stride;
            m_Stride+=element.Stride;
        }
    }

    const std::vector<BufferElement>& GetElements() const{return m_Elements;}

    std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
    std::vector<BufferElement>::iterator end() { return m_Elements.end(); }

    std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
    std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

    const uint32_t GetStride() const {return m_Stride;}

    // static std::shared_ptr<BufferLayout> Create(std::initializer_list<BufferElement> elements){
    //     return std::make_shared<BufferLayout>(elements);
    // }
private:
    std::vector<BufferElement> m_Elements;
    uint32_t m_Stride = 0;
};

class VertexBuffer{
public:
    virtual ~VertexBuffer() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual const BufferLayout& GetLayout() const = 0;
    virtual void SetLayout(BufferLayout& bufferlayout) = 0;

    static std::shared_ptr<VertexBuffer> Create(const void* data, uint32_t size);
};


class IndexBuffer
{
public:
    virtual ~IndexBuffer() = default;

    virtual void Bind() const = 0;
    virtual void UnBind() const = 0;
    virtual std::uint32_t GetCount() const = 0;

    // Span enables various kind of input: vector, array, etc. as long as it is a contiguous sequence of uint32_t
    // the indices can die after the construction
    static std::shared_ptr<IndexBuffer> Create(std::span<const std::uint16_t> indices, uint32_t count);
};

}