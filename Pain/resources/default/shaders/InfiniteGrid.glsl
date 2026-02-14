#shader vertex
#version 400 core

layout(location=0) in vec2 a_Position; // xy only

uniform vec2 u_cameraPos;
uniform mat4 u_ViewProjection;
uniform float u_zoomLevel;

out float v_zoomLevel;
out vec2 v_WorldPos;

void main()
{
    // Create a large quad in world space centered on camera
    // Make it big enough to cover the screen at any zoom level
    float quadSize = u_zoomLevel * 2.0; // Adjust multiplier as needed
    vec2 worldPos = u_cameraPos + a_Position.xy * quadSize;
    
    v_zoomLevel = u_zoomLevel;
    v_WorldPos = worldPos;
    gl_Position = u_ViewProjection * vec4(worldPos, 0.0, 1.0);
}


#shader fragment
#version 400 core

uniform float u_CellSize;
uniform float u_Thickness;
uniform vec3 u_Color;
uniform float u_resolution_y;

in float v_zoomLevel;
in vec2 v_WorldPos;
out vec4 color;


void main()
{
    vec2 p = v_WorldPos; 
    
    // fix grid disappearing when in low res
    float u_Thickness = u_Thickness * 1080.0/u_resolution_y; 
    // BIG GRID -----------------------------------------------------
    float majorEvery = 32.0; 
    float majorCellSize = u_CellSize * majorEvery;

    vec2 posWrappedMajor = mod(p, majorCellSize);
    vec2 gridMajor = abs((posWrappedMajor / majorCellSize) - 0.5);
    float lineDistMajor = min(gridMajor.x, gridMajor.y);

    float majorThickness = u_Thickness * v_zoomLevel / majorCellSize;
    float line = 1.0 - step(majorThickness, lineDistMajor);

    // SMALL GRID -----------------------------------------------------
    vec2 posWrapped = mod(p, u_CellSize);
    vec2 grid = abs(posWrapped / u_CellSize);
    float lineDist = min(grid.x, grid.y);

    float minorThickness = u_Thickness * 0.4 * v_zoomLevel / u_CellSize;
    float lineMinor = 1.0 - step(minorThickness, lineDist);

    float cellScreenPixels = (u_CellSize / v_zoomLevel) * u_resolution_y;
    if (cellScreenPixels > 30.0) {
        line = max(line, lineMinor);
    }
    
    color = vec4(u_Color, 1.0) * line;
}
