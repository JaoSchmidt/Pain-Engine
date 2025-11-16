#shader vertex
#version 400 core

layout(location=0) in vec3 a_Position; // xy only

void main()
{
    gl_Position = vec4(a_Position,  1.0);
}


#shader fragment
#version 400 core

uniform vec2 u_cameraPos;
uniform float u_zoomLevel;
uniform float u_CellSize;
uniform float u_Thickness;
uniform vec3 u_Color;
uniform vec2 u_resolution;

out vec4 color;

void main()
{
    // convert uv to NDC -1..+1
    vec2 uv = gl_FragCoord.xy / u_resolution.xy;
    vec2 ndc = uv * vec2(u_resolution.x / u_resolution.y, 1.0);

    // scale with camera zoom / projection scale if you have it
    // assume 1:1 pixel → world for now
    vec2 p = u_cameraPos+ ndc * u_zoomLevel;


    // BIG GRID -----------------------------------------------------
    float majorEvery = 16.0; 
    float majorCellSize = u_CellSize * majorEvery;

    vec2 gridPosMajor = p / majorCellSize;
    vec2 gridMajor = abs(fract(gridPosMajor - 0.5) - 0.5);
    float lineDistMajor = min(gridMajor.x, gridMajor.y);

    float majorThickness = u_Thickness *0.2/ majorCellSize;
    float line = 1.0 - step(majorThickness, lineDistMajor);


    // SMALL GRID -----------------------------------------------------
    
    vec2 gridPos = p / u_CellSize;
    vec2 grid = abs(fract(gridPos - 0.5) - 0.5);
    float lineDist = min(grid.x, grid.y);
    

    float minorThickness = u_Thickness *0.05 / u_CellSize;
    float lineMinor = 1.0 - step(minorThickness, lineDist);

    float cellScreenPixels = (u_CellSize / u_zoomLevel) * u_resolution.y;
    if (cellScreenPixels > 30.0) {
        line = max(line, lineMinor);
    }
    color = vec4(u_Color,1.0) * line;
}
