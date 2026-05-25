#version 460

layout (set = 0, binding = 1) uniform Bindings{
    vec4 StartColor;
    vec4 EndColor;
};

layout (location = 0) in float Fade;
layout (location = 0) out vec4 Color;

void main()
{
    Color = mix(StartColor, EndColor, Fade);
}