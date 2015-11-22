#version 330 core

layout(location = 0) in vec3 ModelVertices;
layout(location = 1) in vec2 ModelUVs;
layout(location = 2) in vec3 ModelNormals;

out vec2 UV;
out vec3 WorldPos;
out vec3 NormalCameraPos;
out vec3 EyeCameraDirection;
out vec3 LightCameraDirection;
out vec3 Color;

// Projection
uniform mat4 p;

// View
uniform mat4 v;

// Model
uniform mat4 m;

// Light
uniform vec3 l;

// Color
uniform vec3 c;

void main()
{
	Color = c;

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  (p * v * m) * vec4(ModelVertices, 1);
	
	// Position of the vertex, in worldspace : M * position
	WorldPos = (m * vec4(ModelVertices, 1)).xyz;
	
	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 VertexCameraPos = (v * m * vec4(ModelVertices, 1)).xyz;
	EyeCameraDirection = vec3(0, 0, 0) - VertexCameraPos;

	// Vector that goes from the vertex to the light, in camera space.
	vec3 LightPosition_cameraspace = (v * vec4(l, 1)).xyz;
	LightCameraDirection = LightPosition_cameraspace + EyeCameraDirection;
	
	// Normal of the the vertex, in camera space.
	NormalCameraPos = (v * transpose(inverse(m)) * vec4(ModelNormals, 0)).xyz;
	
	// UV of the vertex.
	UV = ModelUVs;
}
