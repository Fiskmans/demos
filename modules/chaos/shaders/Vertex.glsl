#version 460

layout (set = 0, binding = 0) uniform Bindings{
    mat4x4 Transform;
    float LineLength;
};

layout (location = 0) in vec3 Position;
layout (location = 0) out float Fade;

void main()
{
    gl_Position = vec4(Position, 1.0f) * Transform;
    Fade = float(gl_VertexIndex) / LineLength;
}