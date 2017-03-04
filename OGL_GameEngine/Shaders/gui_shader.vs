#version 430

layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out VS_OUT {
out vec2 TexCoords;
} vs_out;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    vs_out.TexCoords = vertex.zw;
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}