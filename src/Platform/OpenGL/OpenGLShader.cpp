#include <filesystem>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OpenGLShader.h"
#include "Adachi/Diagnostics/Debug.h"
#include "Adachi/Diagnostics/Log.h"
#include "Adachi/Diagnostics/Profiler.h"
#include "Adachi/Renderer/UniformValue.h"
#include "Platform/OpenGL/OpenGLDebug.h"

namespace Adachi{

OpenGLShader::OpenGLShader(const char* vertexShaderCode, const char* fragmentShaderCode)
{
    CompileShader(vertexShaderCode, fragmentShaderCode);
}

OpenGLShader::~OpenGLShader(){
    ADACHI_PROFILE_FUNCTION();

    GLCall(glDeleteProgram(m_RendererID));
}

void OpenGLShader::Bind() const
{
    ADACHI_PROFILE_FUNCTION();

    glUseProgram(m_RendererID);
}

void OpenGLShader::Unbind() const
{
    ADACHI_PROFILE_FUNCTION();

    glUseProgram(0);
}



void OpenGLShader::SetUniform(
    GLint location,
    const UniformValue& value)
{
    // A uniform optimized out by the shader has location -1.
    if (location == -1)
        return;

    if (const auto* integer = std::get_if<int>(&value))
    {
        glUniform1i(
            location,
            static_cast<GLint>(*integer));
    }
    else if (const auto* integers =
                 std::get_if<std::vector<int>>(&value))
    {
        if (integers->empty())
            return;

        if (integers->size() >
            static_cast<std::size_t>(
                std::numeric_limits<GLsizei>::max()))
        {
            throw std::length_error(
                "Uniform array is too large.");
        }

        // Convert std::vector<int> into OpenGL's exact integer type.
        const std::vector<GLint> converted(
            integers->begin(),
            integers->end());

        glUniform1iv(
            location,
            static_cast<GLsizei>(converted.size()),
            converted.data());
    }
    else if (const auto* number = std::get_if<float>(&value))
    {
        glUniform1f(
            location,
            static_cast<GLfloat>(*number));
    }
    else if (const auto* vector = std::get_if<glm::vec2>(&value))
    {
        glUniform2fv(
            location,
            1,
            glm::value_ptr(*vector));
    }
    else if (const auto* vector = std::get_if<glm::vec3>(&value))
    {
        glUniform3fv(
            location,
            1,
            glm::value_ptr(*vector));
    }
    else if (const auto* vector = std::get_if<glm::vec4>(&value))
    {
        glUniform4fv(
            location,
            1,
            glm::value_ptr(*vector));
    }
    else if (const auto* matrix = std::get_if<glm::mat3>(&value))
    {
        glUniformMatrix3fv(
            location,
            1,
            GL_FALSE,
            glm::value_ptr(*matrix));
    }
    else if (const auto* matrix = std::get_if<glm::mat4>(&value))
    {
        glUniformMatrix4fv(
            location,
            1,
            GL_FALSE,
            glm::value_ptr(*matrix));
    }
}
void OpenGLShader::CompileShader(const char* vertexShaderCode, const char* fragmentShaderCode)
{
    //ADACHI_CORE_INFO("Shader code: {}", "Hi");
    unsigned int vertexShader{}, fragmentShader{};

    int success{};
    char infoLog[512]{};

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLCall(glShaderSource(vertexShader, 1, &vertexShaderCode, NULL));
    GLCall(glCompileShader(vertexShader));
    GLCall(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success));
    if (!success) {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    }
    ADACHI_CORE_ASSERT(success, "Vertex shader creation failed: {}", infoLog);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLCall(glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL));
    GLCall(glCompileShader(fragmentShader));
    GLCall(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success));
    if (!success) {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      //throw std::runtime_error(infoLog);
    }
    ADACHI_CORE_ASSERT(success, "Fragment shader of creation failed: {}", infoLog);

    m_RendererID = glCreateProgram();
    GLCall(glAttachShader(m_RendererID, vertexShader));
    GLCall(glAttachShader(m_RendererID, fragmentShader));
    GLCall(glLinkProgram(m_RendererID));
    GLCall(glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success));
    if (!success) {
      glGetProgramInfoLog(m_RendererID, 512, NULL, infoLog);
    }
    ADACHI_CORE_ASSERT(success, "Shader linking failed: {}", infoLog);

    GLCall(glDeleteShader(vertexShader));
    GLCall(glDeleteShader(fragmentShader));
}

void OpenGLShader::PrintUniformBlocks(GLint program)
{
    // Uniform blocks used by the current program.
    if (program != 0) {
        GLint blockCount = 0;
        GLint maximumNameLength = 0;

        glGetProgramiv(
            program,
            GL_ACTIVE_UNIFORM_BLOCKS,
            &blockCount
        );
        glGetProgramiv(
            program,
            GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH,
            &maximumNameLength
        );

        std::vector<GLchar> name(
            static_cast<size_t>(std::max(maximumNameLength, 1))
        );

        for (GLint block = 0; block < blockCount; ++block) {
            GLsizei nameLength = 0;
            GLint bindingPoint = 0;
            GLint requiredSize = 0;
            GLint uniformBuffer = 0;
            GLint64 boundOffset = 0;
            GLint64 boundSize = 0;

            glGetActiveUniformBlockName(
                program,
                static_cast<GLuint>(block),
                maximumNameLength,
                &nameLength,
                name.data()
            );

            glGetActiveUniformBlockiv(
                program,
                static_cast<GLuint>(block),
                GL_UNIFORM_BLOCK_BINDING,
                &bindingPoint
            );

            glGetActiveUniformBlockiv(
                program,
                static_cast<GLuint>(block),
                GL_UNIFORM_BLOCK_DATA_SIZE,
                &requiredSize
            );

            glGetIntegeri_v(
                GL_UNIFORM_BUFFER_BINDING,
                static_cast<GLuint>(bindingPoint),
                &uniformBuffer
            );

            glGetInteger64i_v(
                GL_UNIFORM_BUFFER_START,
                static_cast<GLuint>(bindingPoint),
                &boundOffset
            );

            glGetInteger64i_v(
                GL_UNIFORM_BUFFER_SIZE,
                static_cast<GLuint>(bindingPoint),
                &boundSize
            );

            ADACHI_CORE_INFO(
            "Uniform block \"{}\": binding={}, UBO={}, requiredSize={}, "
            "boundOffset={}, boundSize={}",
            name.data(),
            bindingPoint,
            uniformBuffer,
            requiredSize,
            boundOffset,
            boundSize);
        }
    }
}

void OpenGLShader::PrintShaderUniform(GLuint program, const char* name)
{
    const GLint location = glGetUniformLocation(program, name);

    ADACHI_CORE_INFO(
        "Uniform '{}' location: {}",
        name,
        location);

    // The uniform may not exist or may have been optimized out.
    if (location == -1)
        return;

    const GLchar* uniformNames[] = { name };
    GLuint uniformIndex = GL_INVALID_INDEX;

    glGetUniformIndices(
        program,
        1,
        uniformNames,
        &uniformIndex);

    if (uniformIndex == GL_INVALID_INDEX)
    {
        ADACHI_CORE_WARN(
            "Could not find active uniform information for '{}'",
            name);
        return;
    }

    GLint uniformType = 0;
    GLint uniformArraySize = 0;

    glGetActiveUniformsiv(
        program,
        1,
        &uniformIndex,
        GL_UNIFORM_TYPE,
        &uniformType);

    glGetActiveUniformsiv(
        program,
        1,
        &uniformIndex,
        GL_UNIFORM_SIZE,
        &uniformArraySize);

    UniformValue value;

    switch (uniformType)
    {
        case GL_INT:
        case GL_BOOL:
        case GL_SAMPLER_2D:
        case GL_SAMPLER_CUBE:
        {
            if (uniformArraySize == 1)
            {
                GLint result = 0;
                glGetUniformiv(program, location, &result);
                value = static_cast<int>(result);
            }
            else
            {
                std::vector<GLint> results(
                    static_cast<std::size_t>(uniformArraySize));

                glGetUniformiv(
                    program,
                    location,
                    results.data());

                value = std::vector<int>(
                    results.begin(),
                    results.end());
            }

            break;
        }

        case GL_INT_VEC2:
        case GL_BOOL_VEC2:
        {
            std::vector<GLint> results(
                static_cast<std::size_t>(uniformArraySize) * 2);

            glGetUniformiv(program, location, results.data());

            value = std::vector<int>(
                results.begin(),
                results.end());

            break;
        }

        case GL_INT_VEC3:
        case GL_BOOL_VEC3:
        {
            std::vector<GLint> results(
                static_cast<std::size_t>(uniformArraySize) * 3);

            glGetUniformiv(program, location, results.data());

            value = std::vector<int>(
                results.begin(),
                results.end());

            break;
        }

        case GL_INT_VEC4:
        case GL_BOOL_VEC4:
        {
            std::vector<GLint> results(
                static_cast<std::size_t>(uniformArraySize) * 4);

            glGetUniformiv(program, location, results.data());

            value = std::vector<int>(
                results.begin(),
                results.end());

            break;
        }

        case GL_FLOAT:
        {
            if (uniformArraySize != 1)
            {
                ADACHI_CORE_WARN(
                    "Float-array uniform '{}' is not supported by UniformValue",
                    name);
                return;
            }

            GLfloat result = 0.0f;
            glGetUniformfv(program, location, &result);
            value = result;
            break;
        }

        case GL_FLOAT_VEC2:
        {
            if (uniformArraySize != 1)
            {
                ADACHI_CORE_WARN(
                    "vec2-array uniform '{}' is not supported by UniformValue",
                    name);
                return;
            }

            glm::vec2 result{0.0f};
            glGetUniformfv(
                program,
                location,
                glm::value_ptr(result));

            value = result;
            break;
        }

        case GL_FLOAT_VEC3:
        {
            if (uniformArraySize != 1)
            {
                ADACHI_CORE_WARN(
                    "vec3-array uniform '{}' is not supported by UniformValue",
                    name);
                return;
            }

            glm::vec3 result{0.0f};
            glGetUniformfv(
                program,
                location,
                glm::value_ptr(result));

            value = result;
            break;
        }

        case GL_FLOAT_VEC4:
        {
            if (uniformArraySize != 1)
            {
                ADACHI_CORE_WARN(
                    "vec4-array uniform '{}' is not supported by UniformValue",
                    name);
                return;
            }

            glm::vec4 result{0.0f};
            glGetUniformfv(
                program,
                location,
                glm::value_ptr(result));

            value = result;
            break;
        }

        case GL_FLOAT_MAT3:
        {
            if (uniformArraySize != 1)
            {
                ADACHI_CORE_WARN(
                    "mat3-array uniform '{}' is not supported by UniformValue",
                    name);
                return;
            }

            glm::mat3 result{0.0f};
            glGetUniformfv(
                program,
                location,
                glm::value_ptr(result));

            value = result;
            break;
        }

        case GL_FLOAT_MAT4:
        {
            if (uniformArraySize != 1)
            {
                ADACHI_CORE_WARN(
                    "mat4-array uniform '{}' is not supported by UniformValue",
                    name);
                return;
            }

            glm::mat4 result{0.0f};
            glGetUniformfv(
                program,
                location,
                glm::value_ptr(result));

            value = result;
            break;
        }

        default:
        {
            ADACHI_CORE_WARN(
                "Uniform '{}' has unsupported OpenGL type 0x{:X}",
                name,
                static_cast<unsigned int>(uniformType));
            return;
        }
    }

    PrintUniform("GPU", name, value);
}

}