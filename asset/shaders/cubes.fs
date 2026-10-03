#version 410 core

in vec4 v_color0;

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = v_color0;
}