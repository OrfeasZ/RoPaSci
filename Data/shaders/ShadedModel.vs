#version 330 core

layout(location = 0) in vec3 ModelVertices;
layout(location = 1) in vec2 ModelUVs;
layout(location = 2) in vec3 ModelNormals;

out vec2 UV;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

// Values that stay constant for the whole mesh.
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 light;

void main()
{
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  projection * view * model * vec4(ModelVertices, 1);
	
	// Position of the vertex, in worldspace : M * position
	Position_worldspace = (model * vec4(ModelVertices, 1)).xyz;
	
	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vertexPosition_cameraspace = (view * model * vec4(ModelVertices, 1)).xyz;
	EyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace;

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	vec3 LightPosition_cameraspace = (view * vec4(light, 1)).xyz;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
	
	// Normal of the the vertex, in camera space
	Normal_cameraspace = (view * model * vec4(ModelNormals, 0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
	
	// UV of the vertex. No special space for this one.
	UV = ModelUVs;
}
