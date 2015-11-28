#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 WorldPos;
in vec3 NormalCameraPos;
in vec3 EyeCameraDirection;
in vec3 LightCameraDirection;
in vec3 Color;

// Ouput data
out vec4 color;

uniform sampler2D t;

void main()
{
	// Material properties
	vec4 MaterialDiffuseColor = vec4(texture(t, UV).aaa, 1.0) * vec4(Color, 1.0);
	vec4 MaterialAmbientColor = vec4(0.1, 0.1, 0.1, 1.0) * MaterialDiffuseColor;

    vec3 NormalizedNormal = normalize(NormalCameraPos);
    vec3 NormalizedVertexToLightVector = normalize(LightCameraDirection);
 
    // Calculating The Diffuse Term And Clamping It To [0;1]
    float DiffuseTerm = clamp(dot(NormalizedNormal, NormalizedVertexToLightVector), 0.0, 1.0);
 
    // Calculating The Final Color
    color = MaterialAmbientColor + MaterialDiffuseColor * DiffuseTerm;
}