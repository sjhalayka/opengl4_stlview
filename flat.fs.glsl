#version 410 core

// Output
layout (location = 0) out vec4 colour;

in VS_OUT
{
    vec3 vertex_colour;
} fs_in;

void main(void)
{
    // Write final color to the framebuffer
    colour = vec4(fs_in.vertex_colour.r, fs_in.vertex_colour.g, fs_in.vertex_colour.b, 1.0);
}
