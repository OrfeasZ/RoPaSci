#version 330 core

in vec2 TextureCoordinates;

out vec4 color;

uniform sampler2D t;

void main()
{
    color = texture(t, TextureCoordinates) /* * vec4(Color, 1.0) */;
}