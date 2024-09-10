#shader vertex
#version 330 core

layout (location = 0) in vec2 a_Position;  // Position of the quad (particle) in object space
layout (location = 1) in vec2 a_Offset;    // Position offset (where the particle starts)
layout (location = 2) in vec2 u_Normal;          // Directional normal of the rocket base
layout (location = 3) in float a_Time;     // Particle's birth time (for size growth and fading)
layout (location = 4) in float a_RotationSpeed;  // Rotation speed of the particle

uniform mat4 u_ViewProjection;  // Camera view-projection matrix
uniform mat4 u_Transform;       // Camera transform
uniform float u_Time;           // Global time
uniform float u_ParticleVelocity;// Total velocity of the particles
uniform vec2 u_EmiterPos;         // Current position of the emiter


out float v_Lifetime;           // Pass to fragment shader to calculate alpha (fading)
out vec2 v_TexCoord;            // Texture coordinates for the particle quad

// Function to create a 2D rotation matrix
mat2 getRotationMatrix(float angle) {
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);
    return mat2(cosAngle, -sinAngle, sinAngle, cosAngle);
}

void main()
{
    // Calculate particle's age based on the time since it was emitted
    float age = u_Time - a_Time;

    // Offset position based on velocity, normal, and time
    vec2 particlePosition = u_EmiterPos + (u_Normal * age * u_ParticleVelocity);
    
    // Make particles grow over time
    float size = 1.0 + age * 0.2;  // Increase size by 20% per second
    
    // Calculate the particle's rotation angle based on time and rotation speed
    float rotationAngle = a_RotationSpeed * age;
    mat2 rotationMatrix = getRotationMatrix(rotationAngle);

    // Apply rotation and scaling to the quad's vertices
    vec2 rotatedPos = rotationMatrix * (a_Position * size);
    
    // Calculate the final position of the quad (apply offset for particle position)
    vec2 finalPos = rotatedPos + particlePosition + a_Offset;
    
    // Pass lifetime to fragment shader to calculate fading
    v_Lifetime = 1.0 - (age / 5.0);  // Assume 5 seconds for particle lifetime

    // Particle quad texture coordinates (for each vertex)
    v_TexCoord = a_Position * 0.5 + 0.5;  // Map [-1, 1] to [0, 1]

    // Transform the particle using the view-projection matrix
    gl_Position = u_ViewProjection * u_Transform * vec4(finalPos, 0.0, 1.0);
}

#shader fragment
#version 330 core

in float v_Lifetime;    // Particle lifetime for alpha fading
in vec2 v_TexCoord;     // Texture coordinates

uniform sampler2D u_ParticleTexture;  // Texture for the particle (could be a smoke texture)

out vec4 FragColor;

void main()
{
    // Fetch the texture color (assuming a texture with transparency for smooth edges)
    vec4 texColor = texture(u_ParticleTexture, v_TexCoord);
    
    // Fade the particle by decreasing alpha based on lifetime
    float alpha = clamp(v_Lifetime, 0.0, 1.0);
    
    // Make the color grayer over time (fade from white to gray)
    vec3 color = mix(vec3(1.0), vec3(0.5), 1.0 - alpha); // White to gray
    
    // Final particle color with fading alpha
    FragColor = vec4(color, texColor.a * alpha);
}
