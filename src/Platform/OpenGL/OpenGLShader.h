#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include <unordered_map>
#include <utility>

#include "Adachi/Renderer/Shader.h"
#include "Adachi/Renderer/UniformValue.h"
#include "Platform/OpenGL/OpenGLRenderer.h"

namespace Adachi{

class OpenGLShader: public Shader{
public:
	OpenGLShader(const char* vertexShaderCode, const char* fragmentShaderCode);
    ~OpenGLShader();

    void Bind() const override;
	void Unbind() const override;

	GLuint GetID(){return m_RendererID;}
	GLuint GetUniformID(const std::string& name)
	{
		auto it = m_ShaderCache.find(name);

		if(it!=m_ShaderCache.end())
		{
			return it->second;
		}
		else {
			m_ShaderCache[name] = glGetUniformLocation(m_RendererID, name.c_str());
			return m_ShaderCache[name];
		}
	}

	static void PrintCurrentShader();
	static void PrintUniformBlocks(GLint program);
	static void PrintShaderUniform(GLuint program, const char* name);

private:
    void CompileShader(const char* vertexShaderCode, const char* fragmentShaderCode);

	friend OpenGLRenderer;
	void SetUniform(const GLint location, const UniformValue& value);
private:
    uint32_t m_RendererID;

	//Cache
	std::unordered_map<std::string, GLint> m_ShaderCache = {};
};
}