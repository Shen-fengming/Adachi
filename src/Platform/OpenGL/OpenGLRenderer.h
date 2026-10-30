#pragma once

#include "Adachi/Renderer/Buffer.h"
#include "Adachi/Renderer/RenderEnums.h"
#include "Adachi/Renderer/Renderer.h"
#include "Adachi/Core/Window.h"

#include "Adachi/Renderer/Shader.h"
#include "Adachi/Renderer/UniformValue.h"
#include "Adachi/Renderer/UniformBuffer.h"
#include "Adachi/Renderer/VertexArray.h"
#include "OpenGLContext.h"
#include <optional>
#include <unordered_map>

namespace Adachi {

class OpenGLStateCache
{
public:
    std::optional<Rect> viewport;

    std::optional<GLuint> bindedFramebufferID;
    std::optional<GLuint> bindedUniformBufferID;
    std::optional<GLuint> bindedShaderID;
    std::optional<GLuint> bindedVertexArrayID;
    std::optional<GLuint> bindedIndexBufferID;
    std::unordered_map<GLuint, GLuint> bindedTexturesID;    // <texture unit, texture id>

    std::optional<GLenum> topology;

    std::optional<bool> depthTestEnabled;
    std::optional<GLboolean> depthMaskEnabled;
    std::optional<GLenum> depthFunc;

    std::optional<std::array<GLboolean, 4>> colorMask;

    std::unordered_map<GLuint, std::unordered_map<uint32_t, UniformValue>> uniformBuffers;

    // <shader_ID, <uniform_name, <uniform_location, uniform_value>>>
    // for textures, uniform_value is the texture unit
    std::unordered_map<GLuint, std::unordered_map<std::string, std::pair<GLint, UniformValue>>> shadersWithUniforms;

    std::optional<MaterialForCurrentPass>  materialDesc;
};

class OpenGLRenderer: public Renderer{
public:
    explicit OpenGLRenderer(const Window& window);
    ~OpenGLRenderer() override;
public:
    void Init() override;
    void Resize(uint32_t width, uint32_t height) override;

    void BeginFrame() override;
    void EndFrame() override;


    void BeginPass(const RenderPassDesc& description) override;

    void SetupForImGUI() override;
private:
    friend class RenderEncoder;

    void BindShader(Shader& shader) override;
    
    void SetUniformBuffer(UniformBuffer& uniformBuffer, const std::string& name, const UniformValue& uniformValue) override;
    void SetShaderUniform(Shader& shader, const std::string& name, const UniformValue& uniformValue) override;
    void SetShaderTexture(Shader& shader, const std::string& name, Texture& texture) override;
    
    void SetTopology(PrimitiveTopology topology) override;
    void SetDepthBuffer(const DepthSetting& depthSetup) override;
    void SetColorMask(const ColorSetting& setting) override;

    void Draw(VertexArray& vertexArray, IndexBuffer& indexBuffer) override;

    void ApplyFramebufferPreparation(const RenderPassDesc& description);
    
    void PrintCurrentBinding();
    void PrintCurrentViewport();
private:
    OpenGLContext m_Context;
    OpenGLStateCache m_StateCache;

    // weak_ptr<Uniformbuffer>
};

}