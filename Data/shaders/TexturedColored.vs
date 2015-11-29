#version 330 core

out vec2 TextureCoordinates;

// Projection
uniform mat4 p;

// Position
in vec2 v;

// UVs
in vec2 tc;

void main() 
{
	TextureCoordinates = tc;

	gl_Position = p * vec4(v, 0.1, 1.0);
}