#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
out vec2 v_TexCoord;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 u_Model;

out vec3 v_WorldPosition;
out vec3 v_WorldNormal;

void main()
{
    vec4 worldPosition = u_Model * vec4(a_Position, 1.0);

    v_WorldPosition = worldPosition.xyz;

    mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
    v_WorldNormal = normalize(normalMatrix * a_Normal);

    v_TexCoord = a_TexCoord;

    gl_Position = projection * view * worldPosition;

    
}