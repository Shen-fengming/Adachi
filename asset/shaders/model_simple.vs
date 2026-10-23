#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

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

    // Handles non-uniform model scaling correctly.
    mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
    v_WorldNormal = normalize(normalMatrix * a_Normal);

    gl_Position = projection * view * worldPosition;
}