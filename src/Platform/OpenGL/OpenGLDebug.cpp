// Just exist to make clangd stop complaining on GLDebug.h

#include "OpenGLDebug.h"
#include <iostream>

void GLClearError(){
    while(glGetError()!=GL_NO_ERROR){
        // Clear all current errors
    }
}

bool GLLogCall(const char *function, const char *file, int line){
    if(glfwGetCurrentContext() == nullptr){
        std::cerr << "[OpenGL Error] "
        << "(" << "Context is missing" <<")"
        << " " << function
        << " " << file << ":"
        << line
        << std::endl;
        return false;
    }
    while(GLenum error = glGetError() ){
        std::cerr << "[OpenGL Error] "
        << "(" << error <<")"
        << " " << function
        << " " << file << ":"
        << line
        << std::endl;
        return false;
    }
    return true;
}