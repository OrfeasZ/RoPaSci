#version 330 core

in vec2 TextureCoordinates;

out vec4 color;

// Texture
uniform sampler2D t;

// Color
uniform vec3 c;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(t, TextureCoordinates).r);
    color = vec4(c, 1.0) * sampled;
}