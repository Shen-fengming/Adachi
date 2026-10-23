#version 410 core

in vec3 v_WorldPosition;
in vec3 v_WorldNormal;

layout (std140) uniform Light
{
    vec3 u_LightDirection;
};

uniform vec3 u_Color;

out vec4 FragColor;

void main()
{
    vec3 normal = normalize(v_WorldNormal);

    // Direction from the surface toward the light.
    vec3 lightDirection = normalize(-u_LightDirection);

    float diffuseStrength = max(dot(normal, lightDirection), 0.0);

    float ambientStrength = 0.20;
    vec3 lighting = u_Color * (ambientStrength + diffuseStrength * 0.80);

    FragColor = vec4(lighting, 1.0);
}