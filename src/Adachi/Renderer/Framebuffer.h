#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace Adachi{

enum class LoadOperation { Load, Clear, DontCare };

struct ColorAttachmentDescription
{
    LoadOperation loadOperation = LoadOperation::Clear;
    // StoreOperation
    glm::vec4 clearColor { 0.0f, 0.0f, 0.0f, 1.0f };
};

class Framebuffer{
public:
    virtual ~Framebuffer() = default;

    virtual void Bind() = 0;
	virtual void Unbind() = 0;

    //virtual void Resize(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

    static std::shared_ptr<Framebuffer> CreateFramebuffer();

    static std::shared_ptr<Framebuffer> CreateDefault();
};
}