#shader vertex
#version 400 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;

uniform mat4 u_ViewProjection;

out vec2 v_TexCoord;
out vec4 v_Color;
out float v_TexIndex;
out float v_TilingFactor;

void main()
{
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
}

#shader fragment
#version 400 core

layout(location = 0) out vec4 color;
in vec4 v_Color;
in vec2 v_TexCoord;

void main()
{    
	// center around [0,0]
	vec2 p = v_TexCoord * 2.0 - 1.0; // in theory we don't need resolution

	float distance = length(p);
	distance = 1.0 - step(1.0, distance);

	color = vec4(v_Color.rgb , v_Color.a * distance);
							
}
