#shader vertex
#version 330 core

layout (location = 0) in vec2 a_Position;   // -1 or 1 (left/right side of trail)
layout (location = 1) in vec2 a_Point;      // center position of trail node
layout (location = 2) in vec2 a_Direction;  // direction of motion at that node
layout (location = 3) in float a_Time;      // time when this node was created

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform float u_Time;

uniform float u_Width;
uniform float u_LifeTime;

out float v_Age;
out float v_Side;

void main()
{
    float age = u_Time - a_Time;
    v_Age = age;
    v_Side = a_Position.x;

    vec2 dir = normalize(a_Direction);
    vec2 normal = vec2(-dir.y, dir.x);

    vec2 offset = normal * a_Position.x * u_Width;

    vec2 finalPos = a_Point + offset;

    gl_Position = u_ViewProjection * u_Transform * vec4(finalPos, 0.0, 1.0);
}

#shader fragment
#version 330 core

in float v_Age;
in float v_Side;

uniform float u_LifeTime;

out vec4 FragColor;

void main()
{
    vec3 startColor = vec3(1.0,0.6,0.1);
    vec3 endColor = vec3(0.5,0.5,0.5);

    float fade = clamp(v_Age / u_LifeTime, 0.0, 1.0);

    vec3 color = mix(startColor, endColor, fade);

    float alpha = 1.0 - fade;

    FragColor = vec4(color, alpha);
}
