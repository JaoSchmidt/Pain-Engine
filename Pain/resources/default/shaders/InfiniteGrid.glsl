#shader vertex
#version 400 core

layout(location=0) in vec2 a_Position; // xy only
out vec2 v_uv;

void main()
{
    v_uv = (a_Position * 0.5 + 0.5); // 0..1
    gl_Position = vec4(a_Position, 0.0, 1.0);
}


#shader fragment
#version 400 core

uniform vec2 u_CameraPos;   // world
uniform float u_CellSize;
uniform float u_Thickness;
uniform vec3 u_Color;

in vec2 v_uv;
out vec4 color;

void main()
{
    // convert uv to NDC -1..+1
    vec2 ndc = v_uv*2.0 - 1.0;

    // scale with camera zoom / projection scale if you have it
    // assume 1:1 pixel → world for now
    vec2 worldPos = u_CameraPos + ndc;   // ← key idea !

    vec2 gridPos = worldPos / u_CellSize;
    vec2 grid = abs(fract(gridPos - 0.5) - 0.5);
    float lineDist = min(grid.x, grid.y);
    float line = smoothstep(u_Thickness, 0.0, lineDist);

    color = vec4(u_Color,1.0) * line;
}
