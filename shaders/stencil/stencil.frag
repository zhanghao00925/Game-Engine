#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture1;

void main()
{
    color = vec4(0.8, 0.8, 0.8, 1.0);
}