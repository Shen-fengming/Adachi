#version 410 core

in vec3 v_WorldPosition;
in vec3 v_WorldNormal;
in vec2 v_TexCoord;

layout (std140) uniform Light
{
    vec3 u_LightDirection;
};

uniform vec3 u_Color;
uniform sampler2D texture1;

uniform bool u_AlphaClipping;
uniform float u_AlphaCutoff;

out vec4 FragColor;

void main()
{
    vec3 normal = normalize(v_WorldNormal);

    // Direction from the surface toward the light.
    vec3 lightDirection = normalize(-u_LightDirection);

    float diffuseStrength = max(dot(normal, lightDirection), 0.0);

    float ambientStrength = 0.20;

    vec4 base = texture(texture1, v_TexCoord);
    vec3 lighting = base.rgb * u_Color * (ambientStrength + diffuseStrength * 0.80);

    if (u_AlphaClipping && base.a < u_AlphaCutoff)
        discard;

    FragColor = vec4(lighting, base.a);
}