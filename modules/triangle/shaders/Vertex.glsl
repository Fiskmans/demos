#version 460

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 i_color;
layout (location = 0) out vec4 o_color;


void main()
{
    gl_Position = pos;
    o_color = i_color;
}