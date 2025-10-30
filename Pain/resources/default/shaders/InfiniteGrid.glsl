#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Pos; // world-space quad vertex position
uniform mat4 u_ViewProjection;

out vec3 v_WorldPos;

void main()
{
    v_WorldPos = a_Pos;
    gl_Position = u_ViewProjection * vec4(a_Pos, 1.0);
}

#shader fragment
#version 330 core

in vec3 v_WorldPos;
out vec4 FragColor;

uniform float u_CellSize;   // grid spacing, e.g. 1.0
uniform vec3  u_Color;      // line color
uniform vec3  u_Center;     // grid origin
uniform float u_Thickness;  // line thickness in world units

void main()
{
    // shift grid around center
    vec2 gridPos = (v_WorldPos.xy - u_Center.xy) / u_CellSize;

    // fract gives the fractional part [0,1)
    vec2 grid = abs(fract(gridPos - 0.5) - 0.5);

    // distance to nearest line in each axis
    float lineDist = min(grid.x, grid.y);

    // compute grid line intensity
    float line = smoothstep(u_Thickness, 0.0, lineDist);

    vec3 color = mix(vec3(0.0), u_Color, line);
    FragColor = vec4(color, 1.0);
}
