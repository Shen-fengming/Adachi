#version 410 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color0;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 u_model;

out vec4 v_color0;

void main()
{
    gl_Position = projection * view * u_model * vec4(a_position, 1.0);
    v_color0 = a_color0;
}