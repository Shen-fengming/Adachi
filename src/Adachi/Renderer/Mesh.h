#pragma once

#include <memory>
#include <string>
#include <utility>

#include "Adachi/Renderer/Buffer.h"
#include "VertexArray.h"

namespace Adachi {

struct Mesh {
public:
    Mesh(const std::string& name, std::shared_ptr<VertexArray> vertexArray, std::shared_ptr<IndexBuffer> indexBuffer)
    : m_Name(name),m_VertexArrayObject(std::move(vertexArray)), m_IndexBufferObject(std::move(indexBuffer))
    {

    }

    VertexArray& GetVertexArray() const
    {
        return *m_VertexArrayObject;
    }

    IndexBuffer& GetIndexBuffer() const
    {
        return *m_IndexBufferObject;
    } 

    static std::unique_ptr<Mesh> Create(const std::string& name, std::shared_ptr<VertexArray> vertexArray, std::shared_ptr<IndexBuffer> indexBuffer)
    {
        return std::make_unique<Mesh>(name, std::move(vertexArray), std::move(indexBuffer));
    }

private:
    std::string m_Name;
    std::shared_ptr<VertexArray> m_VertexArrayObject;
    std::shared_ptr<IndexBuffer> m_IndexBufferObject;
};

struct MeshAsset
{
    std::string Name;
    std::vector<std::unique_ptr<Mesh>> Meshes;
};

}