#define MAX_LIGHTS 32

struct PointLight
{
    vec4 position; // xyz + radius
    vec4 color;    // rgb
};

uniform int u_PointLightCount;
uniform PointLight u_PointLights[MAX_LIGHTS];


vec3 result = vec3(0.0);

for (int i = 0; i < u_PointLightCount; i++)
{
    vec3 lightPos = u_PointLights[i].position.xyz;
    float radius  = u_PointLights[i].position.w;
    vec3 lightCol = u_PointLights[i].color.rgb;

    vec3 lightDir = lightPos - v_FragPos;
    float distance = length(lightDir);

    if (distance < radius)
    {
        float attenuation = 1.0 - (distance / radius);
        float diff = max(dot(normalize(v_Normal),
                             normalize(lightDir)), 0.0);

        result += lightCol * diff * attenuation;
    }
}

