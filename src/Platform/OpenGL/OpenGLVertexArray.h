#pragma once

#include <glad/gl.h>
#include <iostream>

#include "Adachi/Diagnostics/Log.h"
#include "Adachi/Renderer/Buffer.h"
#include "Adachi/Renderer/VertexArray.h"
#include <memory>

namespace Adachi{
class OpenGLVertexArray: public VertexArray{
public:
    OpenGLVertexArray();
    ~OpenGLVertexArray();

    virtual void Bind() const override;
	virtual void Unbind() const override;

    GLuint GetID(){return m_RendererID;}
    // uint32_t GetCount()
    // // {
    // //     if(m_IndexBuffer)
    // //     {
    // //         return m_IndexBuffer->GetCount();
    // //     }
    // //     return 0;
    // // }

    bool Prepared() const {return m_VertexBuffers.size()>0;}

    // The move in causes the old shared_ptr to point to null
	virtual void AddVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer) override;
	//virtual void SetIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer) override;

    static void PrintOpenGLCurrent();
    
private:
    uint32_t m_RendererID;
    
    std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
    uint32_t m_VertexBufferIndex = 0;                   //The input layout number of shaderprogram

    //std::shared_ptr<IndexBuffer> m_IndexBuffer;
};
}