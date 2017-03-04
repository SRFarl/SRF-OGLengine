#version 430 core

in VS_OUT {
in vec2 TexCoords;
} fs_in;

uniform sampler2D text;
uniform vec3 textColour;

out vec4 color;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, fs_in.TexCoords).r);
    color = vec4(textColour, 1.0) * sampled;
}  