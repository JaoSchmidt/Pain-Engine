#shader vertex
#version 400 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_Coord;    // UVs covering [0,1]

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec4 v_Color;
out vec2 v_UV;

void main()
{
	v_UV = a_Coord;
	v_Color = a_Color;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
}

#shader fragment
#version 400 core

layout(location = 0) out vec4 color;
in vec4 v_Color;
in vec2 v_UV;

void main()
{    
	// center around [0,0]
	vec2 p = v_UV * 2.0 - 1.0; // in theory we don't need resolution

	float distance = length(p);
	distance = 1.0 - step(1.0, distance);

	color = vec4(v_Color.rgb , v_Color.a * distance);
							
}
