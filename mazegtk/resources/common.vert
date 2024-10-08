#version 430 core


layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in vec2 vertexTexCoord;

out vec2 f_tex_pos;

void main()
{
    f_tex_pos = vertexTexCoord;
    gl_Position = vec4(vertexPosition, 0.0, 1.0);
}