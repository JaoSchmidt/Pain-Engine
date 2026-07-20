#shader vertex
#version 400 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in mat4 a_Transform; // 2, 3, 4, 5

uniform mat4 u_ViewProjection;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoord;
out vec4 v_Color;
out float v_TexIndex;
out float v_TilingFactor;

void main()
{
	v_FragPos = a_Position;
	v_Normal = normalize(mat3(a_Transform) * a_Position);
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_TilingFactor = a_TilingFactor;

	gl_Position = u_ViewProjection * a_Transform * vec4(a_Position, 1.0);	
}

#shader fragment
#version 400 core

layout(location = 0) out vec4 color;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;
in float v_TilingFactor;

uniform sampler2D u_Textures[32];

uniform vec3 u_LightPos;
uniform vec4 u_LightColor;
uniform vec3 u_ViewPos;
uniform float u_Ambient; // Strength
uniform float u_Diffuse; // Strength
uniform float u_Highlight; // Strength

void main()
{
  vec3 lightDir = normalize(u_LightPos - v_FragPos);

  // Ambient
  vec3 ambient = u_Ambient * vec3(u_LightColor);

  // Diffuse
  float diff = max(dot(v_Normal, lightDir), 0.0);
  vec3 diffuse = diff * u_Diffuse * vec3(u_LightColor);

  // Specular
  vec3 viewDir = normalize(u_ViewPos - v_FragPos);
  vec3 reflectDir = reflect(-lightDir, v_Normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = u_Highlight * spec * vec3(u_LightColor);

  vec4 texColor = texture(u_Textures[int(v_TexIndex)],
                          v_TexCoord * v_TilingFactor);

  vec3 result = (ambient + diffuse + specular) * texColor.rgb;

  color = vec4(result, texColor.a) * v_Color;

}
