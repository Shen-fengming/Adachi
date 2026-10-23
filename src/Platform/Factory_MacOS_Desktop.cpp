#include "Adachi/Renderer/Framebuffer.h"
#include "Adachi/Renderer/GraphicsAPI.h"

// Factroy pairs
// Window
#include "Adachi/Core/Window.h"
#include "Platform/GLFW/GLFWCreatedWindow.h"
// Renderer
#include "Adachi/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLRenderer.h"

// Buffer
#include "Adachi/Renderer/Buffer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

// VertexArray
#include "Adachi/Renderer/VertexArray.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

// Shader
#include "Adachi/Renderer/Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"

// Framebuffer
#include "Adachi/Renderer/Framebuffer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

// UniformBuffer
#include "Adachi/Renderer/UniformBuffer.h"
#include "Platform/OpenGL/OpenGLUniformbuffer.h"

#include <cstdint>
#include <memory>

namespace Adachi{
GraphicsAPI s_GraphicsAPI = GraphicsAPI::OpenGL;

std::unique_ptr<Window> Window::Create(const Window::Config& config){
    return std::make_unique<GLFWCreatedWindow>(config);
}

std::unique_ptr<Renderer> Renderer::Create(const Window& window){
    return std::make_unique<OpenGLRenderer>(window);
}

std::shared_ptr<VertexBuffer> VertexBuffer::Create(const void *data, uint32_t size, BufferLayout bufferlayout)
{
    return std::make_shared<OpenGLVertexBuffer>(data, size, std::move(bufferlayout));
}

std::shared_ptr<IndexBuffer> IndexBuffer::Create(const void* data, uint32_t count, IndexType indextype)
{
    return std::make_shared<OpenGLIndexBuffer>(data, count, indextype);
}

std::shared_ptr<VertexArray> VertexArray::Create()
{
    return std::make_shared<OpenGLVertexArray>();
}

std::shared_ptr<Shader> Shader::Create(const char* vertexShaderCode, const char* fragmentShaderCode)
{
    return std::make_shared<OpenGLShader>(vertexShaderCode, fragmentShaderCode);
}

std::shared_ptr<Framebuffer> Framebuffer::CreateDefault()
{
    return std::make_shared<OpenGLFramebuffer>(true);
}

std::unique_ptr<UniformBuffer> UniformBuffer::CreateUniformBuffer(UniformBufferLayout &layout, uint32_t bindingPoint)
{
    return std::make_unique<OpenGLUniformbuffer>(layout, bindingPoint);
}

}