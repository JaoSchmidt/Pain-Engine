#shader vertex
#version 400 core
// MSDF text shader
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TexCoord;
out vec4 v_Color;

void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#shader fragment
#version 400 core
layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoord;
in vec4 v_Color;
uniform sampler2D u_FontAtlas;

float screenPxRange() {
	const float pxRange = 2.0; // set to distance field's pixel range
	vec2 unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlas, 0));
	vec2 screenTexSize = vec2(1.0)/fwidth(v_TexCoord);
	return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
	vec4 texColor = v_Color * texture(u_FontAtlas, v_TexCoord);
	vec3 msd = texture(u_FontAtlas, v_TexCoord).rgb;
	float sd = median(msd.r, msd.g, msd.b);
	float screenPxDistance = screenPxRange()*(sd - 0.5);
	float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

	if (opacity == 0.0)
		discard;
	vec4 bgColor = vec4(0.2,0.2,0.2,1.0);
	o_Color = mix(bgColor, v_Color, opacity);
	if (o_Color.a == 0.0)
		discard;

	// o_Color = texture(u_FontAtlas, v_TexCoord) * v_Color;
}
