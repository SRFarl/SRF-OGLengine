#version 330 core

in VS_OUT {
in vec2 TexCoords;
} fs_in;

uniform sampler2D image;

out vec4 color;

void main()
{    
    color = texture(image, fs_in.TexCoords);
	if(color.a < 0.1f)
	{
		discard;
	}
}  