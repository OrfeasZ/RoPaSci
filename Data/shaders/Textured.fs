#version 120

//in vec2 TextureCoordinates;
varying vec2 TextureCoordinates;

//out vec4 color;

uniform sampler2D t;

void main()
{
    //color = texture(t, TextureCoordinates);
    gl_FragColor = texture2D(t, TextureCoordinates);
}