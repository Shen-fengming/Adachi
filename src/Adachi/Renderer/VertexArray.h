#pragma once

#include <memory>

#include "Adachi/Renderer/Buffer.h"

namespace Adachi{
class VertexArray{
public:
    virtual ~VertexArray() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void AddVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer) = 0;     //Can add multiple 
    //virtual void SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer) = 0;      //Can set only one

    static std::shared_ptr<VertexArray> Create();
};
}