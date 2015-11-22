#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
//uniform sampler2D myTextureSampler;

void main()
{
	// Material properties
	//vec3 MaterialDiffuseColor = texture(myTextureSampler, UV).rgb;
	vec3 MaterialDiffuseColor = vec3(1.0, 0.1, 0.1);
	vec3 MaterialAmbientColor = vec3(0.1, 0.1, 0.1) * MaterialDiffuseColor;

    vec3 NormalizedNormal = normalize(Normal_cameraspace);
    vec3 NormalizedVertexToLightVector = normalize(LightDirection_cameraspace);
 
    // Calculating The Diffuse Term And Clamping It To [0;1]
    float DiffuseTerm = clamp(dot(NormalizedNormal, NormalizedVertexToLightVector), 0.0, 1.0);
 
    // Calculating The Final Color
    color = MaterialAmbientColor + MaterialDiffuseColor * DiffuseTerm;
}