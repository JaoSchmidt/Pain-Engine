#shader vertex
#version 330 core

layout (location = 0) in vec2 a_Position;  // Position of the quad (particle) in object space
layout (location = 1) in vec2 a_Offset;    // Position offset (where the particle starts)
layout (location = 2) in vec2 u_Normal;          // Directional normal of the rocket base
layout (location = 3) in float a_Time;     // Particle's birth time (for size growth and fading)
layout (location = 4) in float a_RotationSpeed;  // Rotation speed of the particle

uniform float u_SizeChangeSpeed;
uniform float u_randomSizeFactor;
uniform float u_LifeTime;
uniform mat4 u_ViewProjection;  // Camera view-projection matrix
uniform mat4 u_Transform;       // Camera transform
uniform float u_Time;           // Global time
uniform float u_ParticleVelocity;// Total velocity of the particles
#define M_PI 3.1415926535897932384626433832795

out float v_Age;           // Pass to fragment shader to calculate alpha (fading)
out vec2 v_TexCoord;            // Texture coordinates for the particle quad

float random(vec2 st)
{
	return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

// Function to create a 2D rotation matrix
mat2 getRotationMatrix(float angle) {
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);
    return mat2(cosAngle, -sinAngle, sinAngle, cosAngle);
}

void main()
{
    float age = u_Time - a_Time;
		float rand = random(a_Offset)*0.07;
    float size = 0.02 + rand * u_randomSizeFactor + age * u_SizeChangeSpeed;

    // Offset position based on velocity, normal, and time
		float randNoise = random(u_Normal);
		mat2 rotate90 = getRotationMatrix(randNoise*0.3 + M_PI/2);
    vec2 offSet = a_Offset + ((rotate90 * u_Normal) * age * u_ParticleVelocity*1.5);

    float rotationAngle = a_RotationSpeed * age;
    mat2 rotationMatrix = getRotationMatrix(rotationAngle*3);
    vec2 rotatedScaledPos = rotationMatrix * (a_Position * size);
    vec2 finalPos = rotatedScaledPos + offSet;
    
    v_Age = age;
    v_TexCoord = a_Position * 0.5 + 0.5;  // Map [-1, 1] to [0, 1]
    gl_Position = u_ViewProjection * u_Transform * vec4(finalPos, 0.0, 1.0);
}

#shader fragment
#version 330 core

in float v_Age;    // Particle lifetime for alpha fading
in vec2 v_TexCoord;     // Texture coordinates

uniform float u_LifeTime;             // Particle lifetime duration
uniform sampler2D u_ParticleTexture;  // Texture for the particle (could be a smoke texture)

out vec4 FragColor;

void main()
{   
		vec3 colorStart = vec3(0.9, 0.5, 0.0); // Orange
    vec3 colorEnd = vec3(0.5, 0.5, 0.5);   // Gray

    // Fetch the texture color (assuming a texture with transparency for smooth edges)
    vec4 texColor = texture(u_ParticleTexture, v_TexCoord);
    
    // Fade the particle by decreasing alpha based on lifetime
    float fadeFactor = clamp(v_Age/(u_LifeTime), 0.0, 1.0);
    
    // Make the color grayer over time (fade from white to gray)
    vec3 color = mix(colorStart, colorEnd, fadeFactor);
    
    // Final particle color with fading alpha
    // FragColor = vec4(color, 1 - texColor.a * fadeFactor);
    FragColor = vec4(color, texColor.a * (1.0 - fadeFactor));
}
