#pragma once

#include "Adachi/Renderer/GraphicsContext.h"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

namespace Adachi {

class OpenGLContext: public GraphicsContext{
public:
    explicit OpenGLContext(GLFWwindow* window);
    ~OpenGLContext() override;
    
    void MakeCurrent() override;
    void SwapBuffers() override;

    virtual void SetVSync(bool enabled) override;
    virtual bool IsVSync() const override;
private:
    void Init();

private:
    GLFWwindow* m_Window; 
    bool m_VSyncEnabled;
};

}