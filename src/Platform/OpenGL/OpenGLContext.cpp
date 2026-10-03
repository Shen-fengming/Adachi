// Put it on the top, or the glfw itself will include gl.h again.
// This way glfw can find gl.h has been included and won't do it again
#include <glad/gl.h> 
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

#include "Adachi/Diagnostics/Debug.h"
#include "Adachi/Diagnostics/Profiler.h"

#include "OpenGLContext.h"


namespace Adachi {

OpenGLContext::OpenGLContext(GLFWwindow* window): m_Window(window){
    if(!m_Window){
        ADACHI_CORE_ASSERT(false, "Can not find GLFW window to initialize OpenGL context!");
    }

    Init();
}

OpenGLContext::~OpenGLContext(){}

void OpenGLContext::MakeCurrent(){
    ADACHI_PROFILE_FUNCTION();

    glfwMakeContextCurrent(m_Window);
}

void OpenGLContext::SwapBuffers(){
    ADACHI_PROFILE_FUNCTION();

	glfwSwapBuffers(m_Window);
}

void OpenGLContext::SetVSync(bool enabled){
    ADACHI_PROFILE_FUNCTION();

    m_VSyncEnabled = enabled;

    if(enabled){
        glfwSwapInterval(1);
    }
    else{
        glfwSwapInterval(0);
    }
}

bool OpenGLContext::IsVSync() const{
    return m_VSyncEnabled;
}

#pragma region private helpers
void OpenGLContext::Init(){
    ADACHI_PROFILE_FUNCTION();

    MakeCurrent();

    bool success = gladLoadGL(glfwGetProcAddress);

    ADACHI_CORE_ASSERT(success, "Failed to initialize GLAD!");

    SetVSync(true);

    ADACHI_CORE_INFO("OpenGL Info:");
	ADACHI_CORE_INFO("  Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	ADACHI_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	ADACHI_CORE_INFO("  Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
}
#pragma endregion
}