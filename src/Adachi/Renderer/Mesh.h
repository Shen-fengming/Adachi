#pragma once

#include <memory>
#include <string>
#include <utility>

#include "Adachi/Renderer/Buffer.h"
#include "VertexArray.h"

namespace Adachi {

struct Mesh {
    std::string Name;
    std::shared_ptr<VertexArray> VertexArrayObject;
    std::shared_ptr<IndexBuffer> IndexBufferObject;

    Mesh(const std::string& name, std::shared_ptr<VertexArray> vertexArray, std::shared_ptr<IndexBuffer> indexBuffer): Name(name),VertexArrayObject(std::move(vertexArray)), IndexBufferObject(std::move(indexBuffer))
    {

    }

    static std::shared_ptr<Mesh> CreateMesh(const std::string& name, std::shared_ptr<VertexArray> vertexArray, std::shared_ptr<IndexBuffer> indexBuffer)
    {
        return std::make_shared<Mesh>(name, std::move(vertexArray), std::move(indexBuffer));
    }
};

}