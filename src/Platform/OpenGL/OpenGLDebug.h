//This class should be platform-independent, but now it is dependent on glfw

#pragma once

#pragma region Macros & Functions for debug Opengl calls
#include <glad/gl.h>
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

// DEBUG_BREAK() that stops the program
#if defined(__clang__)
#define DEBUG_BREAK() __builtin_debugtrap()
#else
#define DEBUG_BREAK() std::abort()
#endif

#define ASSERT(condition)  \
    do                     \
    {                      \
        if (!(condition))  \
        {                  \
            DEBUG_BREAK(); \
        }                  \
    } while (false)

// This will be the only way to call OpenGL-side functions
#define GLCall(expression)                          \
    do                                              \
    {                                               \
        GLClearError();                             \
        expression;                                 \
        ASSERT(GLLogCall(#expression, __FILE__, __LINE__)); \
    } while (false)

void GLClearError();
bool GLLogCall(const char *function, const char *file, int line);

#pragma endregion
