#pragma once

#include <cstdint>
#include <memory>

#include "Adachi/Renderer/Buffer.h"
#include "UniformValue.h"
#include "Adachi/Core/Window.h"
#include "Adachi/Diagnostics/Profiler.h"
#include "VertexArray.h"
#include "RenderEnums.h"
#include "UniformBuffer.h"
#include "Shader.h"

#include "Framebuffer.h"


namespace Adachi{

class RenderEncoder;

// The current buffer's relative position& size in the entire window
// turn 0.0-1.0 to 0-10000
class Rect
{
public:
    Rect(float x, float y, float width, float height)
    {
        SetX(x);
        SetY(y);
        SetWidth(width);
        SetHeight(height);
    }

    void SetX(float x){m_X = std::clamp(x, 0.0f,1.0f);}
    void SetY(float y){m_Y = std::clamp(y, 0.0f,1.0f);}
    void SetWidth(float width){m_Width = std::clamp(width , 0.0f,1.0f);}
    void SetHeight(float height){m_Height = std::clamp(height, 0.0f,1.0f);}

    float GetX() const {return m_X;}
    float GetY() const {return m_Y;}
    float GetWidth() const {return m_Width;}
    float GetHeight() const {return m_Height;}

    bool operator==(const Rect&) const = default;
private:
    float m_X;
    float m_Y;
    float m_Width;
    float m_Height;
};


struct RenderPassDesc{
    std::string Name;
    PassRenderTechnique Technique = PassRenderTechnique::Geometry;
    Rect Viewport;
    std::shared_ptr<Framebuffer> Framebuffer;
    std::vector<ColorAttachmentDescription> ColorAttachments;
};

// The material setup for current pass
struct MaterialForCurrentPass
{
    std::shared_ptr<Shader> program;
    PrimitiveTopology topology = PrimitiveTopology::TriangleList;
};

class Renderer{
public: // high level operation
    explicit Renderer(const Window& window): m_Window(window){};
    virtual ~Renderer() = default;

    virtual void Init() = 0;
    //virtual void Render(Scene& scene) = 0;
    
    // Resize should only be changing the window height&width cache, for the actual buffer viewport resize takes place inside the render loop
    virtual void Resize(uint32_t width, uint32_t height) = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;    // reset pipeline description at the end of every pass, 
    virtual void BeginPass(const RenderPassDesc& description) = 0;

    virtual void SetupForImGUI() = 0;

    static std::unique_ptr<Renderer> Create(const Window& window);
protected:
    const Window& m_Window;
    // Upon OnWindowResize(), Changed set to true, Height & Width updated.
    // In the upcoming render loop, the buffers are resized
    // In EndFrame, Changed set to false again
    struct WindowStateCache{
        int Height;
        int Width;
        bool Changed = false;
    };
    WindowStateCache m_WindowStateCache;

protected:// the protected api that get exposed to RenderEncoder
    virtual void SetUniformBuffer(UniformBuffer& uniformBuffer, const std::string& name, const UniformValue& uniformValue) = 0;

    virtual void BindShader(Shader& shader)=0;
    virtual void SetShaderUniform(Shader& shader, const std::string& name, const UniformValue& uniformValue) = 0;
    
    virtual void SetTopology(PrimitiveTopology topology) = 0;
    virtual void SetDepthBuffer(const DepthSetting& depthSetup) = 0;
    virtual void SetColorMask(const ColorSetting& setting) = 0;

    virtual void Draw(VertexArray& vertexArray, IndexBuffer& indexBuffer) = 0;

    
private:// The private api that get exposed to RenderEncoder
    friend class RenderEncoder;
};
}