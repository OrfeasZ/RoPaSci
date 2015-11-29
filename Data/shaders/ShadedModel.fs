#version 120

/*
in vec2 UV;
in vec3 WorldPos;
in vec3 NormalCameraPos;
in vec3 EyeCameraDirection;
in vec3 LightCameraDirection;
in vec3 Color;
*/

varying vec2 UV;
varying vec3 WorldPos;
varying vec3 NormalCameraPos;
varying vec3 EyeCameraDirection;
varying vec3 LightCameraDirection;
varying vec3 Color;

//out vec4 color;

void main()
{
	vec4 MaterialDiffuseColor = vec4(Color, 1.0);
	vec4 MaterialAmbientColor = vec4(0.1, 0.1, 0.1, 1.0) * MaterialDiffuseColor;

    vec3 NormalizedNormal = normalize(NormalCameraPos);
    vec3 NormalizedVertexToLightVector = normalize(LightCameraDirection);
 
    float DiffuseTerm = clamp(dot(NormalizedNormal, NormalizedVertexToLightVector), 0.0, 1.0);
 
    //color = MaterialAmbientColor + MaterialDiffuseColor * DiffuseTerm;
    gl_FragColor = MaterialAmbientColor + MaterialDiffuseColor * DiffuseTerm;
}