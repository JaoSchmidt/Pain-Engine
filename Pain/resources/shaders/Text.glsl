#shader vertex
#version 400 core

layout(location = 0) vec3 a_position;
layout(location = 1) vec4 a_color;
layout(location = 2) vec2 a_texCoord;
layout(location = 3) float a_shift;
layout(location = 4) float a_gamma;

uniform mat4 u_transform;
uniform mat4 u_viewProjection;

out vec4 v_color;
out vec2 v_texCoord;
out float v_shift;
out float v_gamma;

void main()
{
	v_shift = a_shift;
	v_gamma = a_gamma;
	v_color = a_color;
	v_texCoord = a_texCoord;
	gl_Position = u_viewProjection*(u_transform*vec4(a_position,1.0));
}

#shader fragment
#version 400 core

vec3
energy_distribution(vec4 previous, vec4 current, vec4 next)
{
	float primary   = 1.0/3.0;
	float secondary = 1.0/3.0;
	float tertiary  = 0.0;

	// Energy distribution as explained on:
	// http://www.grc.com/freeandclear.htm
	//
	//  .. v..
	// RGB RGB RGB
	// previous.g + previous.b + current.r + current.g + current.b
	//
	//   . .v. .
	// RGB RGB RGB
	// previous.b + current.r + current.g + current.b + next.r
	//
	//     ..v ..
	// RGB RGB RGB
	// current.r + current.g + current.b + next.r + next.g

	float r =
		tertiary  * previous.g +
		secondary * previous.b +
		primary   * current.r  +
		secondary * current.g  +
		tertiary  * current.b;

	float g =
		tertiary  * previous.b +
		secondary * current.r +
		primary   * current.g  +
		secondary * current.b  +
		tertiary  * next.r;

	float b =
		tertiary  * current.r +
		secondary * current.g +
		primary   * current.b +
		secondary * next.r    +
		tertiary  * next.g;

	return vec3(r,g,b);
}

uniform sampler2D u_texture;
uniform vec3 u_pixel;

varying vec4 v_color;
varying vec2 v_texCoord;
varying float v_shift;
varying float v_gamma;

void main()
{
	// LCD Off
	if( u_pixel.z == 1.0)
	{
		float a = texture2D(u_texture, v_texCoord).r;
		gl_FragColor = v_color * pow( a, 1.0/v_gamma );
		return;
	}

	// LCD On
	vec4 current = texture2D(u_texture, v_texCoord);
	vec4 previous= texture2D(u_texture, v_texCoord+vec2(-1.,0.)*u_pixel.xy);
	vec4 next    = texture2D(u_texture, v_texCoord+vec2(+1.,0.)*u_pixel.xy);

	current = pow(current, vec4(1.0/v_gamma));
	previous= pow(previous, vec4(1.0/v_gamma));

	float r = current.r;
	float g = current.g;
	float b = current.b;

	if( v_shift <= 0.333 )
	{
		float z = v_shift/0.333;
		r = mix(current.r, previous.b, z);
		g = mix(current.g, current.r,  z);
		b = mix(current.b, current.g,  z);
	}
	else if( v_shift <= 0.666 )
	{
		float z = (v_shift-0.33)/0.333;
		r = mix(previous.b, previous.g, z);
		g = mix(current.r,  previous.b, z);
		b = mix(current.g,  current.r,  z);
	}
 else if( v_shift < 1.0 )
	{
		float z = (v_shift-0.66)/0.334;
		r = mix(previous.g, previous.r, z);
		g = mix(previous.b, previous.g, z);
		b = mix(current.r,  previous.b, z);
	}

 float t = max(max(r,g),b);
 vec4 color = vec4(v_color.rgb, (r+g+b)/3.0);
 color = t*color + (1.0-t)*vec4(r,g,b, min(min(r,g),b));
 gl_FragColor = vec4( color.rgb, v_color.a*color.a);


	// TODO: Explore this commented section later
	// it's the key for "energy distribution" in text rendering

//    gl_FragColor = vec4(pow(vec3(r,g,b),vec3(1.0/vgamma)),a);

    /*
    vec3 color = energy_distribution(previous, vec4(r,g,b,1), next);
    color = pow( color, vec3(1.0/vgamma));

    vec3 color = vec3(r,g,b); //pow( vec3(r,g,b), vec3(1.0/vgamma));
    gl_FragColor.rgb = color;
    gl_FragColor.a = (color.r+color.g+color.b)/3.0 * vcolor.a;
    */

//    gl_FragColor = vec4(pow(vec3(r,g,b),vec3(1.0/vgamma)),a);
    //gl_FragColor = vec4(r,g,b,a);
}
