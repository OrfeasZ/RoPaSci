#version 120

/*
layout(location = 0) in vec3 ModelVertices;
layout(location = 1) in vec2 ModelUVs;
layout(location = 2) in vec3 ModelNormals;
*/

attribute vec3 mv;
attribute vec2 mu;
attribute vec3 mn;

/*
out vec2 UV;
out vec3 WorldPos;
out vec3 NormalCameraPos;
out vec3 EyeCameraDirection;
out vec3 LightCameraDirection;
out vec3 Color;
*/

varying vec2 UV;
varying vec3 WorldPos;
varying vec3 NormalCameraPos;
varying vec3 EyeCameraDirection;
varying vec3 LightCameraDirection;
varying vec3 Color;

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

	gl_Position =  (p * v * m) * vec4(mv, 1);
	
	WorldPos = (m * vec4(mv, 1)).xyz;
	
	vec3 VertexCameraPos = (v * m * vec4(mv, 1)).xyz;
	EyeCameraDirection = vec3(0, 0, 0) - VertexCameraPos;

	vec3 LightPosition_cameraspace = (v * vec4(l, 1)).xyz;
	LightCameraDirection = LightPosition_cameraspace + EyeCameraDirection;
	
	//NormalCameraPos = (v * transpose(inverse(m)) * vec4(ModelNormals, 0)).xyz;
	NormalCameraPos = (v * m * vec4(mn, 0)).xyz;
	
	UV = mu;
}
