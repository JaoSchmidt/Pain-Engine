#shader vertex
#version 330 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec4 v_Color;
out vec2 v_TexCoord;
void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec2 v_TexCoord;
in vec4 v_Color;

// uniform sampler2D u_Texture;
// uniform vec4 u_Color;
// uniform float u_TilingScale;

void main()
{
	// color = texture(u_Texture, v_TexCoord * u_TilingScale) * u_Color;
	color = v_Color;
}
