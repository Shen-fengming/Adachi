// Put it on the top, or the glfw itself will include gl.h again.
// This way glfw can find gl.h has been included and won't do it again
#include "Adachi/Diagnostics/Log.h"
#include "Platform/OpenGL/OpenGLBuffer.h"
#include <OpenGL/gl.h>
#include <glad/gl.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#include <cstdint>
#include <unordered_map>
#include <utility>

#include "Adachi/Diagnostics/Debug.h"
#include "Adachi/Diagnostics/Profiler.h"
#include "Adachi/Renderer/RenderEnums.h"
#include "Adachi/Renderer/Renderer.h"
#include "OpenGLRenderer.h"
#include "Adachi/Renderer/VertexArray.h"
#include "OpenGLDebug.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/OpenGLUniformbuffer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"


namespace Adachi {
OpenGLRenderer::OpenGLRenderer(const Window &window)
    : Renderer(window),
    m_Context(static_cast<GLFWwindow *>(window.GetNativeHandle())) 
    {}

OpenGLRenderer::~OpenGLRenderer(){}

void OpenGLRenderer::Init()
{
    ADACHI_PROFILE_FUNCTION();

    m_Context.MakeCurrent();
    const auto [width, height] = m_Window.GetFramebufferSize();
    Resize(width, height);
}

void OpenGLRenderer::BeginFrame()
{
    ADACHI_PROFILE_FUNCTION();

    // This should be move to beginpass?
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::EndFrame()
{
    ADACHI_PROFILE_FUNCTION();

    m_WindowStateCache.Changed = false;

    m_Context.SwapBuffers();
}

void OpenGLRenderer::Resize(uint32_t width, uint32_t height)
{
    ADACHI_PROFILE_FUNCTION();

    //glViewport(0, 0, width, height);
    ADACHI_CORE_INFO("Resize: height: {}, width: {}", height, width);

    m_WindowStateCache.Height = height;
    m_WindowStateCache.Width = width;
    m_WindowStateCache.Changed = true;
}

void OpenGLRenderer::BeginPass(const RenderPassDesc& description)
{
    // get the framebuffer id from description.framebuffer
    auto openGLFramebuffer = *(static_cast<OpenGLFramebuffer*>(description.Framebuffer.get()));
    const GLuint framebufferID = openGLFramebuffer.ID();

    // If the frame buffer has changed, bind the new framebuffer & change cache
    if(!m_StateCache.bindedFramebufferID || *m_StateCache.bindedFramebufferID != framebufferID)
    {
        openGLFramebuffer.Bind();
        m_StateCache.bindedFramebufferID = framebufferID;
    }

    // viewport adjustment
    if(!m_StateCache.viewport || *m_StateCache.viewport != description.Viewport || m_WindowStateCache.Changed)
    {
        const Rect& value = description.Viewport;

        uint32_t height = static_cast<uint32_t>((value.GetHeight()/1.0f)*m_WindowStateCache.Height);
        uint32_t width = static_cast<uint32_t>((value.GetWidth()/1.0f)*m_WindowStateCache.Width);
        uint32_t x = static_cast<uint32_t>((value.GetX()/1.0f)*m_WindowStateCache.Width);
        uint32_t y = static_cast<uint32_t>((value.GetY()/1.0f)*m_WindowStateCache.Height);
        ADACHI_CORE_INFO("WindowCache: width:{},height:{}", m_WindowStateCache.Width, m_WindowStateCache.Height);
        ADACHI_CORE_INFO("Viewport: x:{},y:{},width:{},height:{}", value.GetX(), value.GetY(), value.GetWidth(), value.GetHeight());
        ADACHI_CORE_INFO("Viewport: x:{},y:{},width:{},height:{}", x, y, width, height);
        GLCall(glViewport(x,y,width,height));
        //openGLFramebuffer.Resize(x, y, width, height);

        m_StateCache.viewport = value;
    }

    // set color mask/ clear using description.ColorAttachments
    ApplyFramebufferPreparation(description);

    // reset to make the first renderable's material get executed
    m_StateCache.materialDesc.reset();
}

void OpenGLRenderer::SetupForImGUI()
{
    SetColorMask({true, true, true, true});
}

// This function assumes that the value you pass matches the requirement of the buffer
// You need to make sure in the RenderPipeline level
void OpenGLRenderer::SetUniformBuffer(UniformBuffer& uniformBuffer, const std::string& name, const UniformValue& uniformValue)
{
    auto& openGLUniformBuffer =static_cast<OpenGLUniformbuffer&>(uniformBuffer);

    const auto& element = openGLUniformBuffer.GetElementByName(name);
    const GLuint bufferID = openGLUniformBuffer.GetID();

    // Creates the buffer's cache map if it does not exist.
    auto& cachedElements = m_StateCache.uniformBuffers[bufferID];

    auto cachedElement = cachedElements.find(element->Offset);

    if (cachedElement != cachedElements.end() &&
        cachedElement->second == uniformValue)
    {
        return;
    }

    // Binding update
    if (m_StateCache.bindedUniformBufferID != bufferID)
    {
        GLCall(glBindBuffer(GL_UNIFORM_BUFFER, bufferID));
        m_StateCache.bindedUniformBufferID = bufferID;
    }

    // Upload value
    GLCall(glBufferSubData(
        GL_UNIFORM_BUFFER,
        static_cast<GLintptr>(element->Offset),
        static_cast<GLsizeiptr>(element->Size),
        std::addressof(uniformValue)
    ));

    cachedElements.insert_or_assign(element->Offset, uniformValue);
}

void OpenGLRenderer::BindShader(Shader& shader)
{
    auto& openGLShader = static_cast<OpenGLShader&>(shader);
    const GLuint shaderID = openGLShader.GetID();

    auto& cachedShader = m_StateCache.shaders[shaderID];

    if (!m_StateCache.bindedShaderID || m_StateCache.bindedShaderID != shaderID)
    {
        GLCall(glUseProgram(shaderID));
        ADACHI_CORE_INFO("Shader {} Binded", shaderID);
        m_StateCache.bindedShaderID = shaderID;
    }
}

void OpenGLRenderer::SetShaderUniform(Shader& shader, const std::string& name, const UniformValue& uniformValue)
{
    auto& openGLShader = static_cast<OpenGLShader&>(shader);
    const GLuint shaderID = openGLShader.GetID();
    const GLint uniformID = openGLShader.GetUniformID(name);

    auto& cachedShader = m_StateCache.shaders[shaderID];
    auto cachedUniform = cachedShader.find(name);

    if(cachedUniform!=cachedShader.end() && cachedUniform->second.second==uniformValue)
    {
        //ADACHI_CORE_INFO("Shader uniform value unchanged");
        return;
    }

    if (m_StateCache.bindedUniformBufferID != shaderID)
    {
        GLCall(glUseProgram(shaderID));
        m_StateCache.bindedShaderID = shaderID;
    }

    if(cachedUniform!=cachedShader.end())
    {
        //PrintUniform("Cached", name, cachedUniform->second.second);
        openGLShader.SetUniform(cachedUniform->second.first, uniformValue);
        cachedUniform->second.second = uniformValue;
    }
    else 
    {
        auto uniformLocation = glGetUniformLocation(shaderID, name.c_str());
        ADACHI_CORE_INFO("Setting new uniform cache of location: {}", uniformLocation);
        ADACHI_CORE_ASSERT(uniformLocation>=0, "Cannot find location of uniform named {}", name);
        openGLShader.SetUniform(uniformLocation, uniformValue);

        cachedShader.insert_or_assign(name, std::make_pair(uniformLocation, uniformValue));
    }
}

void OpenGLRenderer::Draw(VertexArray& vertexArray, IndexBuffer& indexBuffer)
{
    auto& openGLVertexArray = static_cast<OpenGLVertexArray&>(vertexArray);
    const auto& vertexArrayID = openGLVertexArray.GetID();

    if( !m_StateCache.bindedVertexArrayID || m_StateCache.bindedVertexArrayID != vertexArrayID)
    {
        openGLVertexArray.Bind();
        m_StateCache.bindedVertexArrayID = vertexArrayID;
    }

    auto& openGLIndexBuffer = static_cast<OpenGLIndexBuffer&>(indexBuffer);
    const auto& indexBufferID = openGLIndexBuffer.GetID();

    if( !m_StateCache.bindedIndexBufferID || m_StateCache.bindedIndexBufferID != indexBufferID)
    {
        openGLIndexBuffer.Bind();
        m_StateCache.bindedIndexBufferID = indexBufferID;
    }

    ADACHI_CORE_ASSERT(m_StateCache.topology, "topology not set yet");
    ADACHI_CORE_ASSERT(openGLVertexArray.Prepared(), "VBO is missing on current VAO");

    GLCall(glDrawElements(*m_StateCache.topology, indexBuffer.GetCount(), GL_UNSIGNED_SHORT, 0));

    //OpenGLIndexBuffer::PrintOpenGLCurrent();
    //OpenGLVertexArray::PrintOpenGLCurrent();
};



void OpenGLRenderer::SetTopology(PrimitiveTopology value)
{
    GLenum temp;
    switch (value)
    {
        case PrimitiveTopology::TriangleList:  
            temp =  GL_TRIANGLES;
            break;
        case PrimitiveTopology::TriangleStrip: 
            temp = GL_TRIANGLE_STRIP;
            break;
        case PrimitiveTopology::LineList:      
            temp =  GL_LINES;
            break;
        case PrimitiveTopology::LineStrip:     
            temp =  GL_LINE_STRIP;
            break;
        case PrimitiveTopology::PointList:     
            temp =  GL_POINTS;
            break;
        default: ADACHI_CORE_INFO("OpenGL Topology not found: value = {}", static_cast<int>(value));
    }
    //ADACHI_CORE_INFO("Topology setting: topology = {}", static_cast<int>(temp));

    if(!m_StateCache.topology || temp!=m_StateCache.topology)
    {
        m_StateCache.topology = temp;
        ADACHI_CORE_INFO("Topology changed to {}", static_cast<int>(value));
    }
}

void OpenGLRenderer::SetDepthBuffer(const DepthSetting& depthSetup)
{
    if(!m_StateCache.depthTestEnabled || m_StateCache.depthTestEnabled != depthSetup.enabled)
    {
        if(depthSetup.enabled)
        {
            GLCall(glEnable(GL_DEPTH_TEST));
        }
        else 
        {
            GLCall(glDisable(GL_DEPTH_TEST));
        }
        m_StateCache.depthTestEnabled = depthSetup.enabled;
    }

    GLenum depthMaskEnabled = depthSetup.writeEnabled? GL_TRUE : GL_FALSE;
    if(!m_StateCache.depthMaskEnabled || m_StateCache.depthMaskEnabled != depthMaskEnabled)
    {
        glDepthMask(depthMaskEnabled);
        m_StateCache.depthMaskEnabled = depthMaskEnabled;
    }

    GLenum comparison;
    switch (depthSetup.comparison) {
        case CompareOperation::Never: 
            comparison = GL_NEVER;	
            break;
        case CompareOperation::Less:
            comparison = GL_LESS;	
            break;
        case CompareOperation::LessEqual:
            comparison = GL_LEQUAL;	
            break;
        case CompareOperation::Equal:
            comparison = GL_EQUAL;	
            break;
        case CompareOperation::GreaterEqual:
            comparison = GL_GEQUAL;	
            break;
        case CompareOperation::Greater:
            comparison = GL_GREATER;	
            break;
        case CompareOperation::NotEqual:
            comparison = GL_NOTEQUAL;	
            break;
        case CompareOperation::Always:
            comparison = GL_ALWAYS;
            break;
        default:
            ADACHI_CORE_INFO("OpenGL Topology not found: value = {}", static_cast<int>(depthSetup.comparison));
    }

    if(!m_StateCache.depthFunc || m_StateCache.depthFunc!=comparison)
    {
        glDepthFunc(comparison);
        m_StateCache.depthFunc = comparison;
    }

}

void OpenGLRenderer::SetColorMask(const ColorSetting& setting)
{
    const std::array<GLboolean, 4> value{
        static_cast<GLboolean>(setting.red   ? GL_TRUE : GL_FALSE),
        static_cast<GLboolean>(setting.green ? GL_TRUE : GL_FALSE),
        static_cast<GLboolean>(setting.blue  ? GL_TRUE : GL_FALSE),
        static_cast<GLboolean>(setting.alpha ? GL_TRUE : GL_FALSE)
    };
    if (m_StateCache.colorMask && *m_StateCache.colorMask == value)
        return;
    glColorMask(value[0], value[1], value[2], value[3]);
    m_StateCache.colorMask = value;

    //ADACHI_CORE_INFO("New color mask set in renderer: {},{},{},{}", setting.red, setting.green, setting.blue, setting.alpha);
}

void OpenGLRenderer::ApplyFramebufferPreparation(const RenderPassDesc& description)
{
    for (std::size_t index = 0; index < description.ColorAttachments.size(); ++index)
    {
        const auto& attachment = description.ColorAttachments[index];
        if(attachment.loadOperation == LoadOperation::Clear)
        {
            //m_StateCache.framebuffer.ClearBuffer(&attachment.clearColor.x);
            SetColorMask({true, true, true, true});
            GLCall(glClearBufferfv(GL_COLOR, static_cast<GLint>(index), &attachment.clearColor.x));
        }
    }
}

void OpenGLRenderer::PrintCurrentBinding()
{
    GLint framebuffer = 0;
    GLint drawBuffer = 0;

    GLint currentArrayBuffer = 0;

    GLint program = 0;

    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &framebuffer);
    glGetIntegerv(GL_DRAW_BUFFER0, &drawBuffer);

    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentArrayBuffer);

    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    ADACHI_CORE_INFO("framebuffer: {}", framebuffer);
    ADACHI_CORE_INFO("drawBuffer: {}", drawBuffer);

    ADACHI_CORE_INFO("ubo: {}", currentArrayBuffer);

    ADACHI_CORE_INFO("shader: {}", program);

    OpenGLShader::PrintUniformBlocks(program);
    OpenGLVertexArray::PrintOpenGLCurrent();
    OpenGLIndexBuffer::PrintOpenGLCurrent();
    PrintCurrentViewport();

    std::cout << "=========================\n";
}

void OpenGLRenderer::PrintCurrentViewport()
{
    GLint viewport[4]{};
    glGetIntegerv(GL_VIEWPORT, viewport);

    ADACHI_CORE_INFO("Viewport: {},{},{},{}", viewport[0], viewport[1], viewport[2], viewport[3]);
}

}