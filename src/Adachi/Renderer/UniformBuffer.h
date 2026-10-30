#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace Adachi{

enum class UniformBufferDataType
{
    None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

//size of current data type
constexpr uint32_t UniformBufferDataTypeSize(UniformBufferDataType type)
{
    switch (type)
    {
        case UniformBufferDataType::Float:  return 4;
        case UniformBufferDataType::Float2: return 8;
        case UniformBufferDataType::Float3: return 12;
        case UniformBufferDataType::Float4: return 16;

        case UniformBufferDataType::Mat3:   return 48;
        case UniformBufferDataType::Mat4:   return 64;

        case UniformBufferDataType::Int:    return 4;
        case UniformBufferDataType::Int2:   return 8;
        case UniformBufferDataType::Int3:   return 12;
        case UniformBufferDataType::Int4:   return 16;

        // GLSL bool occupies four bytes in a uniform block.
        case UniformBufferDataType::Bool:   return 4;

        case UniformBufferDataType::None:   return 0;
    }

    return 0;
}

//base alignment
constexpr uint32_t UniformBufferDataTypeAlignment(UniformBufferDataType type)
{
    switch (type)
    {
        case UniformBufferDataType::Float:
        case UniformBufferDataType::Int:
        case UniformBufferDataType::Bool:
            return 4;

        case UniformBufferDataType::Float2:
        case UniformBufferDataType::Int2:
            return 8;

        case UniformBufferDataType::Float3:
        case UniformBufferDataType::Float4:
        case UniformBufferDataType::Mat3:
        case UniformBufferDataType::Mat4:
        case UniformBufferDataType::Int3:
        case UniformBufferDataType::Int4:
            return 16;

        case UniformBufferDataType::None:
            return 0;
    }

    return 0;
}

//aligned offset: 
// In   1.the past offset 2.the alignment of current datatype's alignment
// Out  the new offset that: no less than past offset && can be divided by current alignment
constexpr uint32_t UniformBufferDataAlignTo(uint32_t offset, uint32_t alignment)
{
    if (alignment == 0)
        return offset;

    return ((offset + alignment - 1) / alignment) * alignment;
}

struct UniformBufferElement
{
    std::string Name;
    UniformBufferDataType DataType;
    uint32_t Size;
    uint32_t Alignment;
    uint32_t Offset = 0;

    UniformBufferElement(const std::string& name, UniformBufferDataType dataType):
        Name(name),
        DataType(dataType),
        Alignment(UniformBufferDataTypeAlignment(dataType)),
        Size(UniformBufferDataTypeSize(dataType))
    {}
};



struct UniformBufferLayout{
    UniformBufferLayout(std::initializer_list<UniformBufferElement> elements):Elements(elements){
        CurrentOffset = 0;

        for(auto& element: Elements){
            
            element.Offset = UniformBufferDataAlignTo(CurrentOffset, element.Alignment);
            CurrentOffset = element.Offset + element.Size;
        }
    }

    std::vector<UniformBufferElement> Elements;
    uint32_t CurrentOffset;
};

class UniformBuffer
{
public:
    UniformBuffer(UniformBufferLayout& layout, uint32_t bindingpoint)
        :m_Layout(layout),
        m_BindingPoint(bindingpoint)
        {}
    virtual ~UniformBuffer() = default;

    virtual void SetLayout(UniformBufferLayout& layout) = 0;

    const UniformBufferElement* GetElementByName(const std::string& name)
    {
        for(auto& element: m_Layout.Elements)
        {
            if(element.Name == name)
            {
                return &element;
            }
        }
        return nullptr;
    }

    static std::unique_ptr<UniformBuffer> Create(UniformBufferLayout& layout, uint32_t bindingPoint);

protected:
    UniformBufferLayout m_Layout;
    uint32_t m_BindingPoint;
};
}